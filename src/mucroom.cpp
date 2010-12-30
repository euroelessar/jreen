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
	
	void MUCRoomPrivate::handlePresence(const Presence &pres)
	{
		Q_Q(MUCRoom);
		MUCRoom::Participant part;
		part.d_func()->query = pres.findExtension<MUCRoomUserQuery>();
		if (!part.d_func()->query)
			return;
		emit q->presenceReceived(&part, pres);
	}
	
	void MUCRoomPrivate::handleMessage(const Message &msg)
	{
		Q_Q(MUCRoom);
		if (!msg.subject().isEmpty()) {
			subject = msg.subject();
			emit q->subjectChanged(subject, msg.from().resource());
		}
		if (!msg.body().isEmpty())
			emit q->messageReceived(msg, msg.subtype() != Message::Groupchat);
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
	
}
