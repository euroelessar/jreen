/****************************************************************************
 *  mucroom.h
 *
 *  Copyright (c) 2010-2011 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *  Copyright (c) 2011 by Sidorov Aleksey <sauron@citadelspb.com>
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
#ifndef MUCROOM_H
#define MUCROOM_H

#include <QObject>
#include <QDateTime>
#include "error.h"
#include "disco.h"
#include "presence.h"
#include "message.h"

namespace jreen
{
// xep-0045
class MUCRoomPrivate;
class JREEN_EXPORT MUCRoom : public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(MUCRoom)
	Q_PROPERTY(QString subject READ subject WRITE setSubject NOTIFY subjectChanged)
public:
	enum Affiliation
	{
		AffiliationNone,                /**< No affiliation with the room. */
		AffiliationOutcast,             /**< The user has been banned from the room. */
		AffiliationMember,              /**< The user is a member of the room. */
		AffiliationOwner,               /**< The user is a room owner. */
		AffiliationAdmin,               /**< The user is a room admin. */
		AffiliationInvalid              /**< Invalid affiliation. */
	};
	enum Role
	{
		RoleNone,                       /**< Not present in room. */
		RoleVisitor,                    /**< The user visits a room. */
		RoleParticipant,                /**< The user has voice in a moderatd room. */
		RoleModerator,                  /**< The user is a room moderator. */
		RoleInvalid                     /**< Invalid role. */
	};
	class ParticipantPrivate;
	class JREEN_EXPORT Participant
	{
		Q_DECLARE_PRIVATE(Participant)
	private:
		Participant();
		~Participant();
	public:
		Affiliation affiliation() const;
		Role role() const;
		bool isSelf() const;
		bool isNickChanged() const;
		bool isBanned() const;
		bool isKicked() const;
		QString newNick() const;
		QString reason() const;
		JID realJID() const;
	private:
		QScopedPointer<ParticipantPrivate> d_ptr;
		friend class MUCRoomPrivate;
	};

	MUCRoom(Client *client, const JID &jid);
	virtual ~MUCRoom();

	QString id() const;

	//	inline void setPassword(const QString &password) { m_password = password; }
	//	inline const QString &name() const { return m_jid.node(); }
	//	inline const QString &service() const { return m_jid.domain(); }
	//	inline const QString &nick() const { return m_jid.resource(); }
	void join(Presence::Type type, const QString &message = QString(), int priority = 0);
	void leave(const QString &message = QString());
	void send(const QString &message);
	QString subject() const;
	void setSubject(const QString &subject);
	//	Affiliation affiliation() const { return m_affiliation; }
	//	Role role() const { return m_role; }
	QString nick() const;
	void setNick(const QString &nick);
	//	void setPresence(Presence::Type type, const QString &message = QString(), int priority = 0);
	//	void invite(const JID &jid, const QString &reason = QString(), const QString &thread = QString());
	//	void addHistory(const QString &message, const JID &from, const QDateTime &when);
	//	void setRequestHistory(int value, HistoryRequest type);
	//	void setRequestHistory(const QDateTime &since);
	//	void requestVoice();
	void kick(const QString &nick, const QString &reason = QString()) { setRole(nick, RoleNone, reason); }
	void ban(const QString &nick, const QString &reason = QString()) { setAffiliation(nick, AffiliationOutcast, reason); }
	//	void grantVoice(const QString &nick, const QString &reason = QString()) { setRole(nick, RoleParticipant, reason); }
	//	void revokeVoice(const QString &nick, const QString &reason = QString()) { setRole(nick, RoleVisitor, reason); }
	void setRole(const QString &nick, Role role, const QString &reason = QString());
	void setAffiliation(const QString &nick, Affiliation affiliation, const QString &reason = QString());

public slots:
	void join();
	//	void getRoomInfo();
	//	void getRoomItems();
	void requestRoomConfig();
	void setRoomConfig(const jreen::DataForm::Ptr &form);
signals:
	void presenceReceived(const jreen::Presence &presence, const jreen::MUCRoom::Participant *part);
	void messageReceived(const jreen::Message &message, bool priv);
	void subjectChanged(const QString &subject, const QString &actor);
	void created();
	void joined();
	void leaved();
	void inviteDeclined(const jreen::JID &jid, const QString &reason);
	void error(const jreen::Error::Ptr &error);
	void configurationReceived(const jreen::DataForm::Ptr &form);
	void roomInfoReceived(QSet<QString> features, const QString &name, QSharedPointer<jreen::DataForm> form);
	void roomItemsReceived(const jreen::Disco::ItemList &items);
protected slots:
	void handleIQ(const jreen::IQ &iq, int context);
	void onConnected();
	void onDisconnected();
protected:
	QScopedPointer<MUCRoomPrivate> d_ptr;
};
}

#endif // MUCROOM_H
