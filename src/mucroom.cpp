#include "mucroom_p.h"
#include "client_p.h"

namespace jreen
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

char mucPrivelegesByRole[RolePrivelegesCount][4] = {
	{ 0, 1, 1 },
	{ 0, 1, 1 },
	{ 0, 1, 1 },
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
	void init(const Presence &pres)
	{
		query = pres.findExtension<MUCRoomUserQuery>();
	}

	MUCRoomUserQuery::Ptr query;
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

void MUCRoomPrivate::handlePresence(const Presence &pres)
{
	Q_Q(MUCRoom);
	qDebug() << "handle presence" << pres.from();
	if (Error::Ptr e = pres.findExtension<Error>()) {
		emit q->error(e);
		return;
	}
	MUCRoom::Participant part;
	part.d_func()->query = pres.findExtension<MUCRoomUserQuery>();
	if (!part.d_func()->query)
		return;
	emit q->presenceReceived(pres, &part);
}

void MUCRoomPrivate::handleMessage(const Message &msg)
{
	Q_Q(MUCRoom);
	if (!msg.subject().isEmpty()) {
		subject = msg.subject();
		emit q->subjectChanged(subject, msg.from().resource());
	} else if (!msg.body().isEmpty()) {
		emit q->messageReceived(msg, msg.subtype() != Message::Groupchat);
	}
}

MUCRoom::MUCRoom(Client *client, const JID &jid) : d_ptr(new MUCRoomPrivate(this))
{
	Q_D(MUCRoom);
	d->client = client;
	d->jid = jid;
	d->session = new MUCMessageSession(this);
	ClientPrivate::get(d->client)->rooms.insert(d->jid.bare(), d);
}

MUCRoom::~MUCRoom()
{
	Q_D(MUCRoom);
	ClientPrivate::get(d->client)->rooms.remove(d->jid.bare());
}

QString MUCRoom::id() const
{
	return d_func()->jid.bare();
}

void MUCRoom::join(Presence::Type type, const QString &message, int priority)
{
	Q_D(MUCRoom);
	Presence pres(type, d->jid, message, priority);
	qDebug() << Q_FUNC_INFO << type << d->jid;
	MUCRoomQuery *query = new MUCRoomQuery(d->password);
	query->setMaxChars(d->maxChars);
	query->setMaxStanzas(d->maxStanzas);
	query->setSeconds(d->seconds);
	query->setSince(d->since);
	pres.addExtension(query);
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
	MUCRoomSubmitConfig
};

void MUCRoom::requestRoomConfig()
{
	Q_D(MUCRoom);
	IQ iq(IQ::Get, d->jid.bareJID());
	iq.addExtension(new MUCRoomOwnerQuery);
	d->client->send(iq, this, SLOT(handleIQ(jreen::IQ,int)), MUCRoomRequestConfig);
}

void MUCRoom::setRoomConfig(const jreen::DataForm::Ptr &form)
{
	Q_D(MUCRoom);
	IQ iq(IQ::Get, d->jid.bareJID());
	iq.addExtension(new MUCRoomOwnerQuery(form));
	d->client->send(iq, this, SLOT(handleIQ(jreen::IQ,int)), MUCRoomSubmitConfig);
}

void MUCRoom::leave(const QString &message)
{
	Q_D(MUCRoom);
	Presence pres(Presence::Unavailable, d->jid, message);
	d->client->send(pres);
}

QString MUCRoom::nick() const
{
	return d_func()->jid.resource();
}

void MUCRoom::setNick(const QString &nick)
{
	Q_D(MUCRoom);
	d->jid.setResource(nick);
}

void MUCRoom::setRole(const QString &nick, Role role, const QString &reason)
{
	Q_D(MUCRoom);
	IQ iq(IQ::Set, d->jid.bareJID());
	iq.addExtension(new MUCRoomAdminQuery(nick, role, reason));
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

void MUCRoom::handleIQ(const jreen::IQ &iq, int context)
{
	if (Error::Ptr e = iq.findExtension<Error>()) {
		emit error(e);
		return;
	}
	if (context == MUCRoomRequestConfig) {
		MUCRoomOwnerQuery::Ptr query = iq.findExtension<MUCRoomOwnerQuery>();
		if (!query)
			return;
		emit configurationReceived(query->form);
	}
}
}
