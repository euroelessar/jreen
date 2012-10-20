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

#ifndef JINGLEAUDIOCONTENT_P_H
#define JINGLEAUDIOCONTENT_P_H

#include "jingleaudiocontent.h"
#include "jingleaudiopayload_p.h"
#include "jinglecontent_p.h"

namespace Jreen
{

class JingleAudioDevice : public QIODevice
{
public:
	JingleAudioDevice(JingleAudioContentPrivate *content);
	~JingleAudioDevice();

    virtual bool open(OpenMode mode);
    virtual void close();
	bool isSequential() const;
	qint64 bytesAvailable() const;
	void appendData(const QByteArray &data);

protected:
    virtual qint64 readData(char *data, qint64 len);
    virtual qint64 writeData(const char *data, qint64 len);

private:
	JingleAudioContentPrivate *m_content;
	QByteArray m_buffer;
	QByteArray m_outputBuffer;
};

class JingleAudioContentPrivate : public JingleContentPrivate
{
	Q_DECLARE_PUBLIC(JingleAudioContent)
public:
	JingleAudioContentPrivate(JingleAudioContent *q)
	    : JingleContentPrivate(q), sequence(qrand()), lastSequence(0) {}
	
	quint16 sequence;
	quint16 lastSequence;
	QScopedPointer<JingleAudioDevice> device;
	QList<JingleAudioPayload> payloads;
	QMap<int, JingleAudioCodec*> codecs;
	
	void _q_stateChanged(Jreen::JingleTransport::State);
	void send(int payload, const QByteArray &data);
	static JingleAudioContentPrivate *get(JingleAudioContent *q) { return q->d_func(); }
};

class JingleAudioDescription : public JingleDescription
{
	J_PAYLOAD(Jreen::JingleAudioDescription)
public:
	QList<JingleAudioPayload> payloads;
};

}

#endif // JINGLEAUDIOCONTENT_P_H
