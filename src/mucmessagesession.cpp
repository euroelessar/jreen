/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#include "mucmessagesession_p.h"
#include "mucroom_p.h"
#include "client.h"

namespace Jreen
{
	MUCMessageSession::MUCMessageSession(MUCRoom *room) :
			MessageSession(MUCRoomPrivate::get(room)->client->messageSessionManager(), room->id())
	{
		m_room = MUCRoomPrivate::get(room);
	}
	
	void MUCMessageSession::setSubject(const QString &subject)
	{
		sendMessage(QString(), subject);
	}
	
	void MUCMessageSession::sendMessage(const QString &body, const QString &subject)
	{
		Message message(Message::Groupchat, jid(), body, subject);
		m_manager->send(message);
	}
	
	void MUCMessageSession::handleMessage(const Message &message)
	{
		m_room->handleMessage(message);
	}
}
