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

#include "jinglespeexcodec_p.h"

#ifdef JREEN_HAVE_SPEEX

namespace Jreen
{

JingleSpeexCodec::JingleSpeexCodec(const JingleAudioPayload &payload)
{
	const SpeexMode *mode = 0;
	switch (payload.clockRate()) {
	case 32000:
		mode = &speex_uwb_mode;
		break;
	case 16000:
		mode = &speex_wb_mode;
		break;
	default:
		Q_ASSERT(!"Unknown clockrate");
		// fall through
		// break;
	case 8000:
		mode = &speex_nb_mode;
		break;
	}
	
	speex_bits_init(&m_bits);
    m_encodingState = speex_encoder_init(mode);
	speex_encoder_ctl(m_encodingState, SPEEX_GET_FRAME_SIZE, &m_frameSize); 
	m_decodingState = speex_decoder_init(mode);
	speex_decoder_ctl(m_decodingState, SPEEX_GET_FRAME_SIZE, &m_frameSize);
	
	QString quality = payload.parameter(QLatin1String("quality"));
	if (!quality.isEmpty()) {
		int value = quality.toInt();
		speex_encoder_ctl(m_encodingState, SPEEX_SET_QUALITY, &value);
	}
}

JingleSpeexCodec::~JingleSpeexCodec()
{
	speex_bits_destroy(&m_bits);
	speex_encoder_destroy(m_encodingState);
    speex_decoder_destroy(m_decodingState);
}

int JingleSpeexCodec::frameSize() const
{
	return m_frameSize;
}

QByteArray JingleSpeexCodec::encodeFrame(const char *data, int size)
{
	Q_ASSERT(size == m_frameSize * 2);
	spx_int16_t *inputFrame = reinterpret_cast<spx_int16_t*>(const_cast<char*>(data));
	speex_bits_reset(&m_bits);
	speex_encode_int(m_encodingState, inputFrame, &m_bits);
	QByteArray frame(speex_bits_nbytes(&m_bits), Qt::Uninitialized);
	int frameSize = speex_bits_write(&m_bits, frame.data(), frame.size());
	Q_ASSERT(frameSize == frame.size());
	return frame;
}

QByteArray JingleSpeexCodec::decodeFrame(const char *data, int size)
{
	speex_bits_read_from(&m_bits, const_cast<char *>(data), size);
	QByteArray output(m_frameSize * 2, Qt::Uninitialized);
	spx_int16_t *outputFrame = reinterpret_cast<spx_int16_t*>(output.data());
	speex_decode_int(m_decodingState, &m_bits, outputFrame);
	return output;
}

QList<JingleAudioPayload> JingleSpeexCodecFactory::supportedPayloads()
{
	QList<JingleAudioPayload> result;
	JingleAudioPayload payload;
	payload.setId(97);
	payload.setName(QLatin1String("speex"));
	payload.setClockRate(8000);
	result << payload;
//	payload.setId(96);
//	payload.setClockRate(16000);
//	result << payload;
//	payload.setId(98);
//	payload.setClockRate(32000);
//	result << payload;
	return result;
}

bool JingleSpeexCodecFactory::supportsPayload(const JingleAudioPayload &payload)
{
	return !payload.name().compare(QLatin1String("speex"), Qt::CaseInsensitive)
	        && payload.clockRate() == 8000;
}

JingleAudioCodec *JingleSpeexCodecFactory::createCodec(const JingleAudioPayload &payload)
{
	return new JingleSpeexCodec(payload);
}

}

#endif
