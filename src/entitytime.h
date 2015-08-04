/****************************************************************************
**
** Jreen
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef JREEN_ENTITYTIME_H
#define JREEN_ENTITYTIME_H

#include "stanzaextension.h"
#include <QDateTime>

namespace Jreen
{
	class EntityTimePrivate;
	class JREEN_EXPORT EntityTime : public Payload
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

#endif // JREEN_ENTITYTIME_H
