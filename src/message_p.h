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
#ifndef MESSAGE_P_H
#define MESSAGE_P_H

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

#endif // MESSAGE_P_H
