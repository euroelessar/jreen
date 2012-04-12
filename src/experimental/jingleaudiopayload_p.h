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

#ifndef JINGLEAUDIOPAYLOAD_P_H
#define JINGLEAUDIOPAYLOAD_P_H

#include "jingleaudiopayload.h"
#include <QMap>

namespace Jreen
{

class JingleAudioPayloadData : public QSharedData
{
public:
	JingleAudioPayloadData()
	    : channelCount(1), clockRate(-1), id(-1), maxmimumPacketTime(-1), packetTime(-1) {}
	JingleAudioPayloadData(const JingleAudioPayloadData &o)
	    : QSharedData(o), channelCount(o.channelCount), clockRate(o.clockRate), id(o.id),
	      maxmimumPacketTime(o.maxmimumPacketTime), packetTime(o.packetTime),
	      name(o.name), parameters(o.parameters) {}
	int channelCount;
	int clockRate;
	int id;
	int maxmimumPacketTime;
	int packetTime;
	QString name;
	QMap<QString, QString> parameters;
	
	static const JingleAudioPayloadData *get(const JingleAudioPayload &p) { return p.d.constData(); }
};

}

#endif // JINGLEAUDIOPAYLOAD_P_H
