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
#include "mucroom_p.h"

namespace Jreen
{
enum MUCRolePrivilege
{
	SendMessage,
	ModifySubject,
	KickParticipantsAndVisitors,
	GrantVoice,
	RevokeVoice,
	RolePrivelegesCount
};

static char mucPrivelegesByRole[RolePrivelegesCount][4] = {
	{ 0, 1, 1 },
	{ 0, 1, 1 },
	{ 0, 0, 1 },
	{ 0, 0, 1 },
	{ 0, 0, 1 }
};

bool checkParticipantPrivelege(MUCRolePrivilege priv, MUCRoom::Role role)
{
	return role != MUCRoom::RoleNone && mucPrivelegesByRole[priv][role - 1];
}

//	enum MUCAffiliationPrivilege
//	{
//		SendMessage,
//		ModifySubject,
//		KickParticipantsAndVisitors,
//		GrantVoice,
//		RevokeVoice,
//		RolePrivelegesCount
//	};

//	char mucPrivelegesByAffiliation[RolePrivelegesCount][4] = {
//		{ 0, 1, 1 },
//		{ 0, 1, 1 },
//		{ 0, 1, 1 },
//		{ 0, 0, 1 },
//		{ 0, 0, 1 }
//	};

//	bool checkParticipantPrivelege(MUCAffiliationPrivilege priv, MUCRoom::Affiliation aff)
//	{
//		return aff != MUCRoom::AffiliationOutcast && mucPrivelegesByAffiliation[priv][aff - 1];
//	}

class MUCRoom::ParticipantPrivate
{
public:
	ParticipantPrivate() : joined(false) {}

	void init(const Presence &pres)
	{
		query = pres.payload<MUCRoomUserQuery>();
	}

	MUCRoomUserQuery::Ptr query;
	bool joined;
};

MUCRoom::Participant::Participant() : d_ptr(new ParticipantPrivate)
{
}

MUCRoom::Participant::~Participant()
{
}

MUCRoom::Affiliation MUCRoom::Participant::affiliation() const
{
	return d_func()->query->item.affiliation;
}

MUCRoom::Role MUCRoom::Participant::role() const
{
	return d_func()->query->item.role;
}

bool MUCRoom::Participant::isSelf() const
{
	return d_func()->query->flags & MUCRoomUserQuery::Self;
}

bool MUCRoom::Participant::isNickChanged() const
{
	return d_func()->query->flags & MUCRoomUserQuery::NickChanged;
}

bool MUCRoom::Participant::isBanned() const
{
	return d_func()->query->flags & MUCRoomUserQuery::Banned;
}

bool MUCRoom::Participant::isKicked() const
{
	return d_func()->query->flags & MUCRoomUserQuery::Kicked;
}

bool MUCRoom::Participant::isJoined() const
{
	return d_func()->joined;
}

QString MUCRoom::Participant::newNick() const
{
	return d_func()->query->item.nick;
}

QString MUCRoom::Participant::reason() const
{
	return d_func()->query->item.reason;
}

JID MUCRoom::Participant::realJID() const
{
	return d_func()->query->item.jid;
}

class MUCRoom::ItemPrivate : public QSharedData
{
public:
	ItemPrivate() {}
	ItemPrivate(const ItemPrivate &o) : QSharedData(o), jid(o.jid), reason(o.reason) {}
	JID jid;
	QString reason;
};

MUCRoom::Item::Item(const JID &jid, const QString &reason) : d_ptr(new MUCRoom::ItemPrivate)
{
	d_ptr->jid = jid;
	d_ptr->reason = reason;
}

MUCRoom::Item::Item(const MUCRoom::Item &o) : d_ptr(o.d_ptr)
{
}

MUCRoom::Item &MUCRoom::Item::operator =(const MUCRoom::Item &o)
{
	d_ptr = o.d_ptr;
	return *this;
}

MUCRoom::Item::~Item()
{
}

JID MUCRoom::Item::jid() const
{
	return d_ptr->jid;
}

void MUCRoom::Item::setJID(const JID &jid)
{
	d_ptr->jid = jid;
}

QString MUCRoom::Item::reason() const
{
	return d_ptr->reason;
}

void MUCRoom::Item::setReason(const QString &reason)
{
	d_ptr->reason = reason;
}

void MUCRoomPrivate::handlePresence(const Presence &pres)
{
	Q_Q(MUCRoom);
	Logger::debug() << "handle presence" << pres.from();
	if (Error::Ptr e = pres.payload<Error>()) {
		startedJoining = false;
		emit q->error(e);
		return;
	}
	MUCRoom::Participant part;
	part.d_func()->query = pres.payload<MUCRoomUserQuery>();
	if (!part.d_func()->query)
		return;
	if (pres.subtype() == Presence::Unavailable) {
		participantsHash.remove(pres.from().resource());
	} else {
		if (startedJoining) {
			startedJoining = false;
			QHashIterator<QString,MUCRoomUserQuery::Ptr> it(participantsHash);
			MUCRoom::Participant tmp;
			tmp.d_func()->query = MUCRoomUserQuery::Ptr::create();
			Presence hookPres(Presence::Unavailable, client->jid());
			JID bareJid = jid.bareJID();
			while (it.hasNext()) {
				QString nick = it.next().key();
				hookPres.setFrom(bareJid.withResource(nick));
				emit q->presenceReceived(hookPres, &tmp);
			}
		}
		if (!participantsHash.contains(pres.from().resource()))
			part.d_func()->joined = true;
		participantsHash.insert(pres.from().resource(), part.d_func()->query);
	}
	if (part.isNickChanged() && pres.from().resource() == jid.resource())
		jid.setResource(part.newNick());
	emit q->presenceReceived(pres, &part);
	if (pres.from().resource() == jid.resource()) {
		role = part.role();
		affiliation = part.affiliation();
		if(pres.subtype() == Presence::Unavailable) {
			participantsHash.clear();
			isJoined = false;
			emit q->leaved();
		} else if (!isJoined) {
			isJoined = true;
			emit q->joined();
		}
	}
}

void MUCRoomPrivate::handleMessage(const Message &msg)
{
	Q_Q(MUCRoom);
	bool nice = false;
	bool isPrivate = (msg.subtype() != Message::Groupchat);
	if (msg.from() == jid.bare()) {
		emit q->serviceMessageReceived(msg);
		nice = true;
	}
	if (!msg.subject().isEmpty()) {
		subject = msg.subject();
		emit q->subjectChanged(subject, msg.from().resource());
		nice = true;
	}
	// We want to receive "service" messages like chat states for private sessions
	if (!nice && (isPrivate || !msg.body().isEmpty())) {
		emit q->messageReceived(msg, isPrivate);
	}
}

MUCRoom::MUCRoom(Client *client, const JID &jid) :
	QObject(client),
	d_ptr(new MUCRoomPrivate(this))
{
	Q_D(MUCRoom);
	d->client = client;
	d->jid = jid;
	d->session = new MUCMessageSession(this);
	ClientPrivate::get(d->client)->rooms.insert(d->jid.bare(), d);
	connect(client, SIGNAL(connected()), this, SLOT(onConnected()));
	connect(client, SIGNAL(disconnected(Jreen::Client::DisconnectReason)), this, SLOT(onDisconnected()));
}

MUCRoom::~MUCRoom()
{
	Q_D(MUCRoom);
	if (!d->client)
		return;
	ClientPrivate::get(d->client)->rooms.remove(d->jid.bare());
}

QString MUCRoom::id() const
{
	return d_func()->jid.bare();
}

QString MUCRoom::service() const
{
	return d_func()->jid.domain();
}

void MUCRoom::setPassword(const QString &password)
{
	d_func()->password = password;
}

bool MUCRoom::isJoined() const
{
	return d_func()->isJoined;
}

Presence::Type MUCRoom::presence() const
{
	return d_func()->currentPresence.subtype();
}

void MUCRoom::join(Presence::Type type, const QString &message, int priority)
{
	Q_D(MUCRoom);
	if (d->startedJoining)
		return;
	d->startedJoining = true;
	Presence pres(type, d->jid, message, priority);
	MUCRoomQuery *query = new MUCRoomQuery(d->password);
	query->setMaxChars(d->maxChars);
	query->setMaxStanzas(d->maxStanzas);
	query->setSeconds(d->seconds);
	query->setSince(d->since);
	pres.addExtension(query);
	d->currentPresence = pres;
	d->client->send(pres);
}

void MUCRoom::join()
{
	Q_D(MUCRoom);
	Presence pres = d->client->presence();
	join(pres.subtype(), pres.status(), pres.priority());
}

enum MUCRoomRequestContext
{
	MUCRoomRequestConfig = 100,
	MUCRoomSubmitConfig,
	MUCRoomRequestList,
	MUCRoomEndRequestList = MUCRoomRequestList + 20,
	MUCRoomSetList
};

void MUCRoom::requestRoomConfig()
{
	Q_D(MUCRoom);
	IQ iq(IQ::Get, d->jid.bareJID());
	iq.addExtension(new MUCRoomOwnerQuery);
	d->client->send(iq, this, SLOT(handleIQ(Jreen::IQ,int)), MUCRoomRequestConfig);
}

void MUCRoom::requestList(Jreen::MUCRoom::Affiliation affiliation)
{
	Q_D(MUCRoom);
	IQ iq(IQ::Get, d->jid.bareJID());
	iq.addExtension(new MUCRoomAdminQuery(affiliation));
	d->client->send(iq, this, SLOT(handleIQ(Jreen::IQ,int)), MUCRoomRequestList + affiliation);
}

void MUCRoom::setList(Jreen::MUCRoom::Affiliation affiliation, const Jreen::MUCRoom::ItemList &items)
{
	Q_D(MUCRoom);
	IQ iq(IQ::Set, d->jid.bareJID());
	MUCRoomAdminQuery *query = new MUCRoomAdminQuery;
	if (items.isEmpty())
		return;
	foreach (const Item &item, items) {
		MUCRoomItem tmp;
		tmp.affiliation = affiliation;
		tmp.jid = item.jid();
		tmp.reason = item.reason();
		query->items << tmp;
	}
	iq.addExtension(query);
	d->client->send(iq, this, SLOT(handleIQ(Jreen::IQ,int)), MUCRoomSetList);
}

void MUCRoom::setRoomConfig(const Jreen::DataForm::Ptr &form)
{
	Q_D(MUCRoom);
	IQ iq(IQ::Set, d->jid.bareJID());
	iq.addExtension(new MUCRoomOwnerQuery(form));
	d->client->send(iq, this, SLOT(handleIQ(Jreen::IQ,int)), MUCRoomSubmitConfig);
}

void MUCRoom::leave(const QString &message)
{
	Q_D(MUCRoom);
	if (d->currentPresence.subtype() == Presence::Unavailable)
		return;
	d->isJoined = false;
	Presence pres(Presence::Unavailable, d->jid, message);
	d->currentPresence = pres;
	d->client->send(pres);
}

QString MUCRoom::nick() const
{
	return d_func()->jid.resource();
}

JID MUCRoom::realJid(const QString &nick)
{
	MUCRoomUserQuery::Ptr query = d_func()->participantsHash.value(nick);
	return query ? query->item.jid : JID();
}

void MUCRoom::setNick(const QString &nick)
{
	Q_D(MUCRoom);
	if (d->isJoined) {
		JID newJid = d->jid;
		newJid.setResource(nick);
		Presence pres(d->currentPresence.subtype(), newJid,
					  d->currentPresence.status(), d->currentPresence.priority());
		d->client->send(pres);
	} else {
		d->jid.setResource(nick);
	}
}

void MUCRoom::setHistoryMaxChars(int maxChars)
{
	d_func()->maxChars = maxChars;
}

void MUCRoom::setHistoryMaxStanzas(int maxStanzas)
{
	d_func()->maxStanzas = maxStanzas;
}

void MUCRoom::setHistorySeconds(int seconds)
{
	d_func()->seconds = seconds;
}

void MUCRoom::setHistorySince(const QDateTime &since)
{
	d_func()->since = since;
}

void MUCRoom::setPresence(Presence::Type type, const QString &message, int priority)
{
	Q_D(MUCRoom);
	Presence pres(type, d->jid, message, priority);
	d->client->send(pres);
}

void MUCRoom::invite(const JID &jid, const QString &reason, const QString &thread)
{
	Q_D(MUCRoom);
	if (!d->isJoined || !d->client)
		return;
	Message message(Message::Normal, jid);
	message.addExtension(new MUCRoomUserQuery(MUCRoomUserQuery::Invite, jid, reason, thread));
	d_func()->client->send(message);
}

void MUCRoom::kick(const QString &nick, const QString &reason)
{
	setRole(nick, RoleNone, reason);
}

void MUCRoom::ban(const QString &nick, const QString &reason)
{
	Q_D(MUCRoom);
	MUCRoomUserQuery::Ptr query = d->participantsHash.value(nick);
	JID victim;
	if (!query) {
		// May be it's already full jid, who knows?
		victim = nick;
		if (victim.node().isEmpty() || victim.domain().isEmpty())
			return;
	} else {
		victim = query->item.jid.bareJID();
	}
	setAffiliation(victim, AffiliationOutcast, reason);
}

void MUCRoom::setRole(const QString &nick, Role role, const QString &reason)
{
	Q_D(MUCRoom);
	IQ iq(IQ::Set, d->jid.bareJID());
	iq.addExtension(new MUCRoomAdminQuery(nick, role, reason));
	d->client->send(iq);
}

void MUCRoom::setAffiliation(const JID &jid, Affiliation affiliation, const QString &reason)
{
	Q_D(MUCRoom);
	IQ iq(IQ::Set, d->jid.bareJID());
	iq.addExtension(new MUCRoomAdminQuery(jid, affiliation, reason));
	d->client->send(iq);
}

void MUCRoom::setAffiliation(const QString &nick, Affiliation affiliation, const QString &reason)
{
	Q_D(MUCRoom);
	IQ iq(IQ::Set, d->jid.bareJID());
	iq.addExtension(new MUCRoomAdminQuery(nick, affiliation, reason));
	d->client->send(iq);
}

void MUCRoom::send(const QString &message)
{
	Q_D(MUCRoom);
	d->session->sendMessage(message);
}

QString MUCRoom::subject() const
{
	return d_func()->subject;
}

void MUCRoom::setSubject(const QString &subject)
{
	Q_D(MUCRoom);
	d->session->setSubject(subject);
}

MUCRoom::Affiliation MUCRoom::affiliation() const
{
	return d_func()->affiliation;
}

MUCRoom::Role MUCRoom::role() const
{
	return d_func()->role;
}

bool MUCRoom::canKick(const QString &nick)
{
	Q_D(MUCRoom);
	MUCRoomUserQuery::Ptr query = d->participantsHash.value(nick);
	if (!query)
		return false;
	if (query->item.role == MUCRoom::RoleVisitor || query->item.role == MUCRoom::RoleParticipant)
		return checkParticipantPrivelege(KickParticipantsAndVisitors, d->role);
	return false;
}

bool MUCRoom::canBan(const QString &nick)
{
	Q_D(MUCRoom);
	MUCRoomUserQuery::Ptr query = d->participantsHash.value(nick);
	if (!query)
		return false;
	if (d->affiliation != MUCRoom::AffiliationAdmin && d->affiliation != MUCRoom::AffiliationOwner)
		return false;
	return query->item.affiliation <= MUCRoom::AffiliationMember;
}

void MUCRoom::handleIQ(const Jreen::IQ &iq, int context)
{
	if (Error::Ptr e = iq.payload<Error>()) {
		emit error(e);
		return;
	}
	if (context == MUCRoomRequestConfig) {
		MUCRoomOwnerQuery::Ptr query = iq.payload<MUCRoomOwnerQuery>();
		if (!query)
			return;
		emit configurationReceived(query->form);
	} else if (context >= MUCRoomRequestList && context < MUCRoomEndRequestList) {
		ItemList items;
		MUCRoomAdminQuery::Ptr query = iq.payload<MUCRoomAdminQuery>();
		if (!query)
			return;
		foreach (const MUCRoomItem &item, query->items) {
			if (!item.jid.isValid())
				continue;
			items << Item(item.jid, item.reason);
		}
		Affiliation affiliation = static_cast<Affiliation>(context - MUCRoomRequestList);
		emit listReceived(affiliation, items);
	}
}

void MUCRoom::onConnected()
{
	Q_D(MUCRoom);
	if (d->currentPresence.subtype() != Presence::Unavailable)
		join(d->currentPresence.subtype(), d->currentPresence.status(), d->currentPresence.priority());
}

void MUCRoom::onDisconnected()
{
	Q_D(MUCRoom);
	d->startedJoining = false;
	if (d->currentPresence.subtype() != Presence::Unavailable) {
		d->participantsHash.clear();
		d->isJoined = false;
		emit leaved();
	}
}
}
