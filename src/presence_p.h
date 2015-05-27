/****************************************************************************
**
** Jreen
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#ifndef JREEN_PRESENCE_P_H
#define JREEN_PRESENCE_P_H

#include "stanza_p.h"
#include "presence.h"
#include "langmap.h"

namespace Jreen
{
class PresencePrivate : public StanzaPrivate
{
public:
	PresencePrivate()
	    : StanzaPrivate(StanzaPresence),
	      subtype(Presence::Available),
	      priority(0) {}
	Presence::Type subtype;
	LangMap status;
	int priority;
};
}

#endif // JREEN_PRESENCE_P_H
