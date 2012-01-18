/****************************************************************************
**
** Jreen
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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
#ifndef CHATSTATE_H
#define CHATSTATE_H
#include "stanzaextension.h"

namespace Jreen {

class ChatStatePrivate;

//XEP-0085 Chat State Notification
//http://xmpp.org/extensions/xep-0085.html
class JREEN_EXPORT ChatState : public Payload
{
	Q_DECLARE_PRIVATE(ChatState)
	J_PAYLOAD(Jreen::ChatState)
public:
	enum State
	{
		Active = 0,    // User is actively participating in the chat session.
		InActive,      // User has not been actively participating in the chat session.
		Gone,          // User has effectively ended their participation in the chat session.
		Composing,     // User is composing a message.
		Paused         // User had been composing but now has stopped.
	};
	ChatState(State state);
	~ChatState();
	State state() const;
private:
	QScopedPointer<ChatStatePrivate> d_ptr;
};

} // namespace Jreen
Q_ENUMS(Jreen::ChatState::State)

#endif // CHATSTATE_H
