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
#ifndef JREEN_MESSAGE_P_H
#define JREEN_MESSAGE_P_H

#include "stanza_p.h"
#include "langmap.h"
#include "message.h"

namespace Jreen
{

class MessagePrivate : public StanzaPrivate
{
public:
	MessagePrivate() : StanzaPrivate(StanzaMessage), subtype(Message::Normal) {}
	Message::Type subtype;
	LangMap body;
	LangMap subject;
	QString thread;
};

} //namespace Jreen

#endif // JREEN_MESSAGE_P_H
