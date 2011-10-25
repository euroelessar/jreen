/****************************************************************************
 *  chatstate.h
 *
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/
#ifndef CHATSTATE_H
#define CHATSTATE_H
#include "stanzaextension.h"

namespace Jreen {

//XEP-0085 Chat State Notification
//http://xmpp.org/extensions/xep-0085.html
class ChatState : public Payload
{
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
	ChatState(State state) : m_state(state) {}
	ChatState() {}
	inline State state() const { return m_state; }
private:
	State m_state;
};

} // namespace Jreen
Q_ENUMS(Jreen::ChatState::State)

#endif // CHATSTATE_H
