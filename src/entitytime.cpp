/****************************************************************************
**
** Jreen
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "entitytime.h"

namespace Jreen
{
	class EntityTimePrivate
	{
	public:
		int tzo;
		QDateTime utc;
	};
	
	EntityTime::EntityTime(int tzo, const QDateTime &dateTime) : d_ptr(new EntityTimePrivate)
	{
		Q_D(EntityTime);
		d->tzo = tzo;
		d->utc = dateTime.toUTC();
	}
	
	EntityTime::EntityTime(const QDateTime &dateTime) : d_ptr(new EntityTimePrivate)
	{
		Q_D(EntityTime);
		QDateTime local = dateTime.toLocalTime();
		local.setTimeSpec(Qt::UTC);
		d->utc = dateTime.toUTC();
		d->tzo = d->utc.secsTo(local) / 60;
	}
	
	EntityTime::EntityTime() : d_ptr(new EntityTimePrivate)
	{
		Q_D(EntityTime);
		d->tzo = 0;
	}

	EntityTime::~EntityTime()
	{
	}
	
	int EntityTime::timeZoneOffset() const
	{
		return d_func()->tzo;
	}
	
	QDateTime EntityTime::localTime() const
	{
		Q_D(const EntityTime);
		QDateTime dateTime = d->utc;
		dateTime.addSecs(60 * d->tzo);
		dateTime.setTimeSpec(Qt::LocalTime);
		return dateTime;
	}
	
	QDateTime EntityTime::universalTime() const
	{
		return d_func()->utc;
	}
}
