/****************************************************************************
**
** Jreen
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $JREEN_BEGIN_LICENSE$
** Jreen is free software: you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** Jreen is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with Jreen.  If not, see <http://www.gnu.org/licenses/>.
** $JREEN_END_LICENSE$
**
****************************************************************************/

#if !defined(JREEN_JINGLESPEEXCODEC_P_H) && defined(JREEN_HAVE_SPEEX)
#define JREEN_JINGLESPEEXCODEC_P_H

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

#endif // JREEN_JINGLESPEEXCODEC_P_H
