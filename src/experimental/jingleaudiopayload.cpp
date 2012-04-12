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

#include "jingleaudiopayload_p.h"

namespace Jreen
{

Q_GLOBAL_STATIC_WITH_ARGS(QSharedDataPointer<JingleAudioPayloadData>, nullData, (new JingleAudioPayloadData))

JingleAudioPayload::JingleAudioPayload() : d(*nullData())
{
}

JingleAudioPayload::JingleAudioPayload(const JingleAudioPayload &o) : d(o.d)
{
}

JingleAudioPayload &JingleAudioPayload::operator =(const JingleAudioPayload &o)
{
	d = o.d;
	return *this;
}

JingleAudioPayload::~JingleAudioPayload()
{
}

bool JingleAudioPayload::operator ==(const JingleAudioPayload &o)
{
	return (o.id() < 96 && d->id == o.id())
	        || (d->channelCount == o.channelCount()
	            && d->clockRate == o.clockRate()
	            && d->name == o.name());
}

bool JingleAudioPayload::operator !=(const JingleAudioPayload &o)
{
	return !operator ==(o);
}

int JingleAudioPayload::channelCount() const
{
	return d->channelCount;
}

void JingleAudioPayload::setChannelCount(int channelCount)
{
	d->channelCount = (channelCount <= 0 ? 1 : channelCount);
}

int JingleAudioPayload::clockRate() const
{
	return d->clockRate;
}

void JingleAudioPayload::setClockRate(int clockRate)
{
	d->clockRate = (clockRate <= 0 ? -1 : clockRate);
}

int JingleAudioPayload::id() const
{
	return d->id;
}

void JingleAudioPayload::setId(int id)
{
	d->id = id;
}

int JingleAudioPayload::maximumPacketTime() const
{
	return d->maxmimumPacketTime;
}

void JingleAudioPayload::setMaximumPacketTime(int maximumPacketTime)
{
	d->maxmimumPacketTime = (maximumPacketTime <= 0 ? -1 : maximumPacketTime);
}

QString JingleAudioPayload::name() const
{
	return d->name;
}

void JingleAudioPayload::setName(const QString &name)
{
	d->name = name;
}

int JingleAudioPayload::packetTime() const
{
	return d->packetTime;
}

void JingleAudioPayload::setPacketTime(int packetTime)
{
	d->packetTime = (packetTime <= 0 ? -1 : packetTime);
}

QString JingleAudioPayload::parameter(const QString &name) const
{
	return d->parameters.value(name);
}

void JingleAudioPayload::setParameter(const QString &name, const QString &value)
{
	d->parameters.insert(name, value);
}

}
