/****************************************************************************
**
** Jreen
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $JREEN_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $JREEN_END_LICENSE$
**
****************************************************************************/

#include "jingleaudiocontent_p.h"
#include "jinglespeexcodec_p.h"
#include <QSet>
#include <QtEndian>
#include <QDateTime>
#include <QDebug>
#include <qmath.h>
#include "../logger.h"

namespace Jreen
{

enum { JingleRTP = 0, JingleRTCP = 1 };

class JingleRtpHeader
{
public:
	JingleRtpHeader() : begin(2 << 6), pt(0), sq(0), ts(0), ssrc(0) {}
	JingleRtpHeader(const char * &data, int &len);
	enum { Version = 2 };
	
	bool isValid() const { return version() == 2; }
	quint8 version() const { return begin >> 6; }
	bool hasPadding() const { return begin & (1 << 5); }
	bool hasExtension() const { return begin & (1 << 4); }
	quint8 csrcCount() const { return begin & 0x7; }
	bool hasMarker() const { return begin & (1 << 7); }
	quint8 payloadType() const { return pt /*& 0xef*/ ; }
	void setPayloadType(quint8 type) { /* pt &= ~0xef; pt |= type;*/ pt = type; }
	quint16 sequence() const { return sq; }
	void setSequence(quint16 seq) { sq = seq; }
	quint32 timestamp() const { return ts; }
	void setTimestamp(quint32 timest) { ts = timest; }
	quint32 synchronizationSource() const { return ssrc; }
	QByteArray data() const;
	
private:
	quint8 begin;
	quint8 pt;
	quint16 sq;
	quint32 ts;
	quint32 ssrc;
};

JingleRtpHeader::JingleRtpHeader(const char * &xdata, int &len)
{
	const uchar * &udata = reinterpret_cast<const uchar * &>(xdata);
	const uchar *start = udata;
	begin  = udata[0];
	pt     = udata[1];
	sq     = qFromBigEndian<quint16>(udata + 2);
	ts     = qFromBigEndian<quint32>(udata + 4);
	ssrc   = qFromBigEndian<quint32>(udata + 4);
	udata += 12;
	udata += csrcCount() * 4;
	len -= (udata - start);
}

QByteArray JingleRtpHeader::data() const
{
	QByteArray result(1 + 1 + 2 + 4 + 4, Qt::Uninitialized);
	uchar *d = reinterpret_cast<uchar*>(result.data());
	d[0] = begin;
	d[1] = pt;
	qToBigEndian(sq,   d + 2);
	qToBigEndian(ts,   d + 4);
	qToBigEndian(ssrc, d + 8);
	return result;
}

JingleAudioDevice::JingleAudioDevice(JingleAudioContentPrivate *content)
    : m_content(content)
{
}

JingleAudioDevice::~JingleAudioDevice()
{
}

bool JingleAudioDevice::open(OpenMode mode)
{
	Q_UNUSED(mode);
	return QIODevice::open(QIODevice::ReadWrite | QIODevice::Unbuffered);
}

void JingleAudioDevice::close()
{
	m_buffer.clear();
	m_outputBuffer.clear();
	QIODevice::close();
}

bool JingleAudioDevice::isSequential() const
{
	return true;
}

qint64 JingleAudioDevice::bytesAvailable() const
{
	return m_outputBuffer.size();
}

void JingleAudioDevice::appendData(const QByteArray &data)
{
	m_outputBuffer.append(data);
	if (m_outputBuffer.size() > (8 * 320))
		m_outputBuffer.remove(0, m_outputBuffer.size() - 8 * 320);
	emit readyRead();
}

qint64 JingleAudioDevice::readData(char *data, qint64 maxSize)
{
	qMemSet(data, 0, maxSize);
	qint64 size = qMin<qint64>(m_outputBuffer.size(), maxSize);
	qMemCopy(data, m_outputBuffer.data(), size);
	m_outputBuffer.remove(0, size);
	return size;
}

qint64 JingleAudioDevice::writeData(const char *data, qint64 len)
{
	m_buffer.append(data, len);
	const JingleAudioPayload &payload = m_content->payloads.first();
	JingleAudioCodec *codec = m_content->codecs.value(payload.id());
	// We use only two-byte integers right now
	const int frameSize = codec->frameSize() * 2;
	int offset = 0;
	while (m_buffer.size() - offset >= frameSize) {
		m_content->send(payload.id(), codec->encodeFrame(m_buffer.data() + offset, frameSize));
		offset += frameSize;
	}
	m_buffer.remove(0, offset);
	return len;
}

static inline void init_factories(QList<JingleAudioCodecFactory*> &factories)
{
	Q_UNUSED(factories);
#ifdef JREEN_HAVE_SPEEX
	factories << new JingleSpeexCodecFactory;
#endif
}

Q_GLOBAL_STATIC_WITH_INITIALIZER(QList<JingleAudioCodecFactory*>, factories, init_factories(*x))

JingleAudioContent::JingleAudioContent(JingleSession *session)
    : JingleContent(session, *new JingleAudioContentPrivate(this))
{
	Q_D(JingleAudioContent);
	d->device.reset(new JingleAudioDevice(d));
	setComponentCount(2);
}

JingleAudioContent::~JingleAudioContent()
{
	qDeleteAll(d_func()->codecs);
}

JingleAudioPayload JingleAudioContent::currentPayload() const
{
	return d_func()->payloads.value(0);
}

int JingleAudioContent::currentPayloadFrameSize() const
{
	Q_D(const JingleAudioContent);
	int id = d->payloads.value(0).id();
	JingleAudioCodec *codec = d->codecs.value(id);
	return codec ? codec->frameSize() : -1;
}

QIODevice *JingleAudioContent::audioDevice() const
{
	return d_func()->device.data();
}

JingleDescription::Ptr JingleAudioContent::defaultDescription()
{
	JingleAudioDescription::Ptr info = JingleAudioDescription::Ptr::create();
	foreach (JingleAudioCodecFactory *factory, *factories())
		info->payloads << factory->supportedPayloads();
	return info;
}

JingleDescription::Ptr JingleAudioContent::handleDescription(const JingleDescription::Ptr &description)
{
	Q_D(JingleAudioContent);
	JingleAudioDescription::Ptr info = description.staticCast<JingleAudioDescription>();
	QSet<int> payloads;
	foreach (JingleAudioCodecFactory *factory, *factories()) {
		for (int i = 0; i < info->payloads.size(); ++i) {
			if (payloads.contains(i))
				continue;
			const JingleAudioPayload &payload = info->payloads.at(i);
			if (factory->supportsPayload(payload)) {
				d->payloads << payload;
				JingleAudioCodec *codec = factory->createCodec(payload);
				d->codecs.insert(payload.id(), codec);
				payloads.insert(i);
			}
		}
	}
	if (d->payloads.isEmpty())
		return JingleDescription::Ptr();
	JingleAudioDescription::Ptr result = JingleAudioDescription::Ptr::create();
	result->payloads = d->payloads;
	if (!d->payloads.isEmpty())
		emit payloadChoosed(d->payloads.value(0));
	return result;
}

void JingleAudioContent::registerCodec(JingleAudioCodecFactory *factory)
{
	factories()->append(factory);
}

void JingleAudioContentPrivate::_q_stateChanged(Jreen::JingleTransport::State newState)
{
	if (newState == JingleTransport::Connected)
		device->open(QIODevice::ReadWrite);
	JingleContentPrivate::_q_stateChanged(newState);
}

void JingleAudioContentPrivate::send(int payload, const QByteArray &data)
{
	JingleRtpHeader header;
	header.setSequence(sequence++);
	header.setTimestamp(QDateTime::currentDateTime().toTime_t());
	header.setPayloadType(payload);
	QByteArray result = header.data();
	result += data;
	q_func()->send(JingleRTP, result);
}

void JingleAudioContent::receive(int component, const QByteArray &receivedData)
{
	if (component == JingleRTCP) {
		Logger::debug() << Q_FUNC_INFO << receivedData.toHex();
	}
	if (component != JingleRTP)
		return;
	Q_D(JingleAudioContent);
	const char *data = receivedData.constData();
	int size = receivedData.size();
	JingleRtpHeader header(data, size);
	if (!header.isValid())
		return;
	JingleAudioCodec *codec = d->codecs.value(header.payloadType());
	if (!codec)
		return;
	QByteArray audio = codec->decodeFrame(data, size);
	d->device->appendData(audio);
}

}

#include "moc_jingleaudiocontent.cpp"
