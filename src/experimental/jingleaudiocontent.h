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

#ifndef JINGLEAUDIOCONTENT_H
#define JINGLEAUDIOCONTENT_H

#include "jinglecontent.h"
#include "jingleaudiopayload.h"

namespace Jreen
{

class JingleAudioContentPrivate;
class JingleAudioCodecFactory;

class JREEN_EXPORT JingleAudioContent : public JingleContent
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(JingleAudioContent)
public:
    JingleAudioContent(JingleSession *session);
	~JingleAudioContent();
	
	JingleAudioPayload currentPayload() const;
	int currentPayloadFrameSize() const;
	QIODevice *audioDevice() const;
	
	virtual JingleDescription::Ptr defaultDescription();
	virtual JingleDescription::Ptr handleDescription(const JingleDescription::Ptr &description);
	
	static void registerCodec(JingleAudioCodecFactory *factory);
	
protected:
	virtual void receive(int component, const QByteArray &data);
	
signals:
	void payloadChoosed(const Jreen::JingleAudioPayload &payload);
};

class JREEN_EXPORT JingleAudioCodec
{
public:
	virtual ~JingleAudioCodec() {}

	virtual int frameSize() const = 0;
	virtual QByteArray encodeFrame(const char *data, int size) = 0;
	virtual QByteArray decodeFrame(const char *data, int size) = 0;
};

class JREEN_EXPORT JingleAudioCodecFactory
{
public:
	virtual ~JingleAudioCodecFactory() {}
	virtual QList<JingleAudioPayload> supportedPayloads() = 0;
	virtual bool supportsPayload(const JingleAudioPayload &payload) = 0;
	virtual JingleAudioCodec *createCodec(const JingleAudioPayload &payload) = 0;
};

}

Q_DECLARE_INTERFACE(Jreen::JingleAudioCodecFactory, "org.qutim.Jreen.Jingle.AudioCodecFactory")

#endif // JINGLEAUDIOCONTENT_H
