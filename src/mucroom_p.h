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

#ifndef MUCROOM_P_H
#define MUCROOM_P_H

#include "mucroom.h"
#include "mucmessagesession_p.h"

namespace jreen
{
	class MUCRoomPrivate
	{
	public:
		Q_DECLARE_PUBLIC(MUCRoom)
		
		MUCRoomPrivate(MUCRoom *room) :
				q_ptr(room), client(0), affiliation(MUCRoom::AffiliationOutcast),
				role(MUCRoom::RoleNone), maxChars(-1), maxStanzas(-1), seconds(-1)
		{
		}
		
		static MUCRoomPrivate *get(MUCRoom *room) { return room->d_func(); }
		void handlePresence(const Presence &pres);
		void handleMessage(const Message &msg);
		
		MUCRoom *q_ptr;
		Client *client;
		JID jid;
		MUCRoom::Affiliation affiliation;
		MUCRoom::Role role;
		QString password;
		int maxChars;
		int maxStanzas;
		int seconds;
		QDateTime since;
		MUCMessageSession *session;
		QString subject;
	};
	
	class MUCRoomQuery : public StanzaExtension
	{
		J_EXTENSION(jreen::MUCRoomQuery, "")
	public:
		MUCRoomQuery(const QString &password)
			: m_password(password), m_maxChars(-1), m_maxStanzas(-1), m_seconds(-1) {}
		void setMaxChars(int maxChars) { m_maxChars = qMax(-1, maxChars); }
		void setMaxStanzas(int maxStanzas) { m_maxStanzas = qMax(-1, maxStanzas); }
		void setSeconds(int seconds) { m_seconds = qMax(-1, seconds); }
		void setSince(const QDateTime &since) { m_since = since; }
	private:
		QString m_password;
		int m_maxChars;
		int m_maxStanzas;
		int m_seconds;
		QDateTime m_since;
		friend class MUCRoomQueryFactory;
	};
	
	class MUCRoomUserQuery : public StanzaExtension
	{
		J_EXTENSION(jreen::MUCRoomUserQuery, "")
	public:
		JID nick;
		MUCRoom::Affiliation affiliation;
		MUCRoom::Role role;
		JID jid;
		int flags;
		QString reason;
		JID actor;
		QString newNick;
		QString status;
		JID alternate;
	};
}

#endif // MUCROOM_P_H
