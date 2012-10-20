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

#if !defined(JINGLESPEEXCODEC_P_H) && defined(JREEN_HAVE_SPEEX)
#define JINGLESPEEXCODEC_P_H

#include "jingleaudiocontent.h"
#include <speex/speex.h>

namespace Jreen
{

class JingleSpeexCodec : public JingleAudioCodec
{
public:
	JingleSpeexCodec(const JingleAudioPayload &payload);
	~JingleSpeexCodec();
	
	virtual int frameSize() const;
	virtual QByteArray encodeFrame(const char *data, int size);
	virtual QByteArray decodeFrame(const char *data, int size);

private:
	SpeexBits m_bits;
	void *m_decodingState;
    void *m_encodingState;
	int m_frameSize;
};

class JingleSpeexCodecFactory : public JingleAudioCodecFactory
{
public:
	virtual QList<JingleAudioPayload> supportedPayloads();
	virtual bool supportsPayload(const JingleAudioPayload &payload);
	virtual JingleAudioCodec *createCodec(const JingleAudioPayload &payload);
};

}

#endif // JINGLESPEEXCODEC_P_H
