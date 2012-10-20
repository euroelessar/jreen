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

#include "jingleaudiocontentfactory_p.h"

#define NS_RTP QLatin1String("urn:xmpp:jingle:apps:rtp:1")
#define NS_RTP_AUDIO QLatin1String("urn:xmpp:jingle:apps:rtp:audio")

namespace Jreen
{

JingleAudioContentFactory::JingleAudioContentFactory()
    : JingleContentFactory<JingleAudioDescription>(NS_RTP, QLatin1String("audio")), m_state(AtRoot), m_depth(0)
{
}

JingleContent *JingleAudioContentFactory::createObject(JingleSession *session)
{
	return new JingleAudioContent(session);
}

QStringList JingleAudioContentFactory::features() const
{
	return QStringList(m_elementUri)
	        << NS_RTP_AUDIO;
}

void JingleAudioContentFactory::handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(name);
	Q_UNUSED(uri);
	m_depth++;
	if (m_depth == 1) {
		m_info = JingleAudioDescription::Ptr::create();
	} if (m_depth == 2 && name == QLatin1String("payload-type")) {
		m_state = AtPayload;
		m_payload.reset(new JingleAudioPayload);
		m_payload->setId(attributes.value(QLatin1String("id")).toString().toInt());
		m_payload->setChannelCount(attributes.value(QLatin1String("channels")).toString().toInt());
		m_payload->setClockRate(attributes.value(QLatin1String("clockrate")).toString().toInt());
		m_payload->setName(attributes.value(QLatin1String("name")).toString());
		m_payload->setMaximumPacketTime(attributes.value(QLatin1String("maxptime")).toString().toInt());
		m_payload->setPacketTime(attributes.value(QLatin1String("ptime")).toString().toInt());
	} else if (m_depth == 3 && name == QLatin1String("parameter")) {
		m_payload->setParameter(attributes.value(QLatin1String("name")).toString(),
		                        attributes.value(QLatin1String("value")).toString());
	}
}

void JingleAudioContentFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	Q_UNUSED(name);
	Q_UNUSED(uri);
	if (m_state == AtPayload && m_depth == 2) {
		m_state = AtRoot;
		m_info->payloads << *m_payload;
		m_payload.reset();
	}
	m_depth--;
}

void JingleAudioContentFactory::handleCharacterData(const QStringRef &text)
{
	Q_UNUSED(text);
}

void JingleAudioContentFactory::serialize(Payload *obj, QXmlStreamWriter *writer)
{
	JingleAudioDescription *info = se_cast<JingleAudioDescription*>(obj);
	Q_ASSERT(info);
	
	writer->writeStartElement(QLatin1String("description"));
	writer->writeDefaultNamespace(m_elementUri);
	writer->writeAttribute(QLatin1String("media"), m_media);
	foreach (const JingleAudioPayload &payload, info->payloads) {
		const JingleAudioPayloadData *d = JingleAudioPayloadData::get(payload);
		writer->writeStartElement(QLatin1String("payload-type"));
		writer->writeAttribute(QLatin1String("id"), QString::number(d->id));
		if (d->channelCount > 0)
			writer->writeAttribute(QLatin1String("channels"), QString::number(d->channelCount));
		if (d->clockRate > 0)
			writer->writeAttribute(QLatin1String("clockrate"), QString::number(d->clockRate));
		if (!d->name.isEmpty())
			writer->writeAttribute(QLatin1String("name"), d->name);
		if (d->maxmimumPacketTime > 0)
			writer->writeAttribute(QLatin1String("maxptime"), QString::number(d->maxmimumPacketTime));
		if (d->packetTime > 0)
			writer->writeAttribute(QLatin1String("ptime"), QString::number(d->packetTime));
		QMapIterator<QString, QString> it(d->parameters);
		while (it.hasNext()) {
			it.next();
			writer->writeEmptyElement(QLatin1String("parameter"));
			writer->writeAttribute(QLatin1String("name"), it.key());
			writer->writeAttribute(QLatin1String("value"), it.value());
		}
		writer->writeEndElement();
	}
	writer->writeEndElement();
}

Payload::Ptr JingleAudioContentFactory::createPayload()
{
	Payload::Ptr result = m_info;
	m_info.clear();
	return result;
}

}
