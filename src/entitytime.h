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

#ifndef ENTITYTIME_H
#define ENTITYTIME_H

#include "stanzaextension.h"
#include <QDateTime>

namespace Jreen
{
	class EntityTimePrivate;
	class EntityTime : public Payload
	{
		J_PAYLOAD(Jreen::EntityTime)
		Q_DECLARE_PRIVATE(EntityTime)
	public:
		EntityTime(int tzo, const QDateTime &dateTime);
		EntityTime(const QDateTime &dateTime);
		EntityTime();
		~EntityTime();
		
		int timeZoneOffset() const;
		QDateTime localTime() const;
		QDateTime universalTime() const;
	private:
		QScopedPointer<EntityTimePrivate> d_ptr;
	};
}

#endif // ENTITYTIME_H
