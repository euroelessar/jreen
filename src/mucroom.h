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
#ifndef MUCROOM_H
#define MUCROOM_H

#include <QObject>
#include <QDateTime>
#include "error.h"
#include "disco.h"
#include "presence.h"
#include "message.h"

namespace Jreen
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
	class ItemPrivate;
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
		bool isJoined() const;
		QString newNick() const;
		QString reason() const;
		JID realJID() const;
	private:
		QScopedPointer<ParticipantPrivate> d_ptr;
		friend class MUCRoomPrivate;
	};
	class JREEN_EXPORT Item
	{
	public:
		Item(const JID &jid, const QString &reason);
		Item(const Item &o);
		Item &operator =(const Item &o);
		~Item();
		
		JID jid() const;
		void setJID(const JID &jid);
		QString reason() const;
		void setReason(const QString &reason);
	private:
		QSharedDataPointer<ItemPrivate> d_ptr;
	};
	typedef QList<Item> ItemList;

	MUCRoom(Client *client, const JID &jid);
	virtual ~MUCRoom();

	QString id() const;
	QString service() const;

	void setPassword(const QString &password);
	//	inline const QString &name() const { return m_jid.node(); }
	//	inline const QString &service() const { return m_jid.domain(); }
	//	inline const QString &nick() const { return m_jid.resource(); }
	bool isJoined() const;
	Presence::Type presence() const;
	void join(Presence::Type type, const QString &message = QString(), int priority = 0);
	void leave(const QString &message = QString());
	void send(const QString &message);
	QString subject() const;
	void setSubject(const QString &subject);
	Affiliation affiliation() const;
	Role role() const;
	bool canKick(const QString &nick);
	bool canBan(const QString &nick);
	QString nick() const;
	JID realJid(const QString &nick);
	void setNick(const QString &nick);
	void setHistoryMaxChars(int maxChars);
	void setHistoryMaxStanzas(int maxStanzas);
	void setHistorySeconds(int seconds);
	void setHistorySince(const QDateTime &since);
	void setPresence(Presence::Type type, const QString &message = QString(), int priority = 0);
	void invite(const JID &jid, const QString &reason = QString(), const QString &thread = QString());
	//	void addHistory(const QString &message, const JID &from, const QDateTime &when);
	//	void setRequestHistory(int value, HistoryRequest type);
	//	void setRequestHistory(const QDateTime &since);
	//	void requestVoice();
	void kick(const QString &nick, const QString &reason);
	void ban(const QString &nick, const QString &reason);
	//	void grantVoice(const QString &nick, const QString &reason = QString()) { setRole(nick, RoleParticipant, reason); }
	//	void revokeVoice(const QString &nick, const QString &reason = QString()) { setRole(nick, RoleVisitor, reason); }
	void setRole(const QString &nick, Role role, const QString &reason = QString());
	void setAffiliation(const JID &jid, Affiliation affiliation, const QString &reason = QString());
	void setAffiliation(const QString &nick, Affiliation affiliation, const QString &reason = QString());

public slots:
	void join();
	//	void getRoomInfo();
	//	void getRoomItems();
	void requestRoomConfig();
	void requestList(Jreen::MUCRoom::Affiliation affiliation);
	// Remember that items is delta. That means that items should contain only changed items
	void setList(Jreen::MUCRoom::Affiliation affiliation, const Jreen::MUCRoom::ItemList &items);
	void setRoomConfig(const Jreen::DataForm::Ptr &form);
signals:
	void presenceReceived(const Jreen::Presence &presence, const Jreen::MUCRoom::Participant *part);
	void messageReceived(const Jreen::Message &message, bool priv);
	void serviceMessageReceived(const Jreen::Message &message);
	void subjectChanged(const QString &subject, const QString &actor);
	void created();
	void joined();
	void leaved();
	void inviteDeclined(const Jreen::JID &jid, const QString &reason);
	void error(const Jreen::Error::Ptr &error);
	void configurationReceived(const Jreen::DataForm::Ptr &form);
	void roomInfoReceived(QSet<QString> features, const QString &name, QSharedPointer<Jreen::DataForm> form);
	void roomItemsReceived(const Jreen::Disco::ItemList &items);
	void listReceived(Jreen::MUCRoom::Affiliation affiliation, const Jreen::MUCRoom::ItemList &items);
protected slots:
	void handleIQ(const Jreen::IQ &iq, int context);
	void onConnected();
	void onDisconnected();
protected:
	QScopedPointer<MUCRoomPrivate> d_ptr;
};
}

#endif // MUCROOM_H
