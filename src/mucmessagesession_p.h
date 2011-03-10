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

#ifndef MUCMESSAGESESSION_P_H
#define MUCMESSAGESESSION_P_H

#include "messagesession.h"

namespace Jreen
{
	class MUCRoom;
	class MUCRoomPrivate;
	
	class MUCMessageSession : public MessageSession
	{
		Q_OBJECT
	public:
		explicit MUCMessageSession(MUCRoom *room);
		
		void setSubject(const QString &subject);
		
		void sendMessage(const QString &body, const QString &subject = QString());
		void handleMessage(const Message &message);
	private:
		MUCRoomPrivate *m_room;
	};
}

#endif // MUCMESSAGESESSION_P_H
