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
