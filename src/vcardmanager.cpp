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

#include "vcardmanager.h"
#include "iqreply.h"
#include "iq.h"
#include "client.h"
#include "vcardupdate.h"

namespace Jreen
{
class VCardManagerPrivate
{
	Q_DECLARE_PUBLIC(VCardManager)
public:
	VCardManagerPrivate(VCardManager *q) : q_ptr(q) {}
	void _q_received(const Jreen::Presence &);
	
	VCardManager *q_ptr;
	Client *client;
	QHash<JID, VCardReply*> hash;
};

void VCardManagerPrivate::_q_received(const Jreen::Presence &presence)
{
	if (presence.error())
		return;
	if (VCardUpdate::Ptr update = presence.payload<VCardUpdate>())
		emit q_ptr->vCardUpdateDetected(presence.from(), update);
}

VCardManager::VCardManager(Client *client)
    : QObject(client), d_ptr(new VCardManagerPrivate(this))
{
	Q_D(VCardManager);
	d->client = client;
	connect(d->client, SIGNAL(presenceReceived(Jreen::Presence)),
	        SLOT(_q_received(Jreen::Presence)));
	connect(d->client, SIGNAL(mucPresenceReceived(Jreen::Presence)),
	        SLOT(_q_received(Jreen::Presence)));
}

VCardManager::~VCardManager()
{
}

VCardReply *VCardManager::fetch(const JID &jid)
{
	Q_D(VCardManager);
	VCardReply *reply = d->hash.value(jid);
	if (!reply) {
		IQ iq(IQ::Get, jid);
		iq.addExtension(new VCard);
		reply = new VCardReply(jid, this, d_func()->client->send(iq));
		connect(reply, SIGNAL(vCardFetched(Jreen::VCard::Ptr,Jreen::JID)),
				this, SIGNAL(vCardFetched(Jreen::VCard::Ptr,Jreen::JID)));
		d->hash.insert(jid, reply);
	}
	return reply;
}

VCardReply *VCardManager::store(const Jreen::VCard::Ptr &vcard)
{
	Q_D(VCardManager);
	IQ iq(IQ::Set, JID());
	iq.addExtension(vcard);
	return new VCardReply(d->client->jid().bareJID(), NULL, d->client->send(iq));
}

void VCardManager::notifyReplyDeath(const JID &jid)
{
	d_func()->hash.remove(jid);
}

class VCardReplyPrivate
{
	Q_DECLARE_PUBLIC(VCardReply)
public:
	VCardReplyPrivate(VCardReply *q) : q_ptr(q) {}
	void _q_received(const Jreen::IQ &iq);
	
	VCardReply *q_ptr;
	QWeakPointer<VCardManager> manager;
	JID jid;
	VCard::Ptr vcard;
	Error::Ptr error;
};

void VCardReplyPrivate::_q_received(const Jreen::IQ &iq)
{
	if (iq.error()) {
		error = iq.error();
		emit q_ptr->error(error);
	} else if ((vcard = iq.payload<Jreen::VCard>())) {
		emit q_ptr->vCardFetched(vcard, jid);
	}
	emit q_ptr->finished();
}

VCardReply::VCardReply(const JID &jid, VCardManager *manager, IQReply *reply)
    : QObject(reply), d_ptr(new VCardReplyPrivate(this))
{
	Q_D(VCardReply);
	d->manager = manager;
	d->jid = jid;
	connect(reply, SIGNAL(received(Jreen::IQ)), SLOT(_q_received(Jreen::IQ)));
}

VCardReply::~VCardReply()
{
	Q_D(VCardReply);
	if (d->manager)
		d->manager.data()->notifyReplyDeath(d->jid);
}

JID VCardReply::jid() const
{
	return d_func()->jid;
}

VCard::Ptr VCardReply::vcard() const
{
	return d_func()->vcard;
}

Error::Ptr VCardReply::error() const
{
	return d_func()->error;
}
}

#include "moc_vcardmanager.cpp"
