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

#ifndef JINGLEAUDIOPAYLOAD_H
#define JINGLEAUDIOPAYLOAD_H

#include "../jreen.h"
#include <QSharedDataPointer>

namespace Jreen
{

class JingleAudioPayloadData;

class JREEN_EXPORT JingleAudioPayload
{
public:
	JingleAudioPayload();
	JingleAudioPayload(const JingleAudioPayload &o);
	JingleAudioPayload &operator =(const JingleAudioPayload &o);
	~JingleAudioPayload();
	
	bool operator ==(const JingleAudioPayload &o);
	bool operator !=(const JingleAudioPayload &o);
	
	int channelCount() const;
	void setChannelCount(int channelCount);
	int clockRate() const;
	void setClockRate(int clockRate);
	int id() const;
	void setId(int id);
	int maximumPacketTime() const;
	void setMaximumPacketTime(int maximumPacketTime);
	QString name() const;
	void setName(const QString &name);
	int packetTime() const;
	void setPacketTime(int packetTime);
	QString parameter(const QString &name) const;
	void setParameter(const QString &name, const QString &value);

private:
	QSharedDataPointer<JingleAudioPayloadData> d;
	friend class JingleAudioPayloadData;
};

}

#endif // JINGLEAUDIOPAYLOAD_H
