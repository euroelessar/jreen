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

#include "abstractroster_p.h"
#include "client.h"
#include "jstrings.h"
#include "iq.h"
#include "client_p.h"
#include "jid.h"
#include <QXmlStreamWriter>
#include "logger.h"
#include "util.h"

#define NS_ROSTER QLatin1String("jabber:iq:roster")

namespace Jreen
{

static const char *subscription_types[] = {
    "from",
    "to",
    "both",
    "remove",
    "none"
};

using namespace Util;

AbstractRosterQueryFactory::AbstractRosterQueryFactory(AbstractRoster *roster)
{
	m_roster = roster;
	m_state = AtStart;
	m_depth = 0;
}

QStringList AbstractRosterQueryFactory::features() const
{
	return QStringList(NS_ROSTER);
}

bool AbstractRosterQueryFactory::canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(attributes);
	return name == QLatin1String("query") && uri == NS_ROSTER;
}

void AbstractRosterQueryFactory::handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(uri);
	m_depth++;
	if (m_depth == 1) {
		m_items.clear();
		m_ver = attributes.value(QLatin1String("ver")).toString();
		m_state = AtQuery;
	} else if (m_depth == 2 && name == QLatin1String("item")) {
		m_state = AtItem;
		m_jid = JID(attributes.value(QLatin1String("jid")).toString()).bare();
		m_name = attributes.value(QLatin1String("name")).toString();
		m_ask = attributes.value(QLatin1String("ask")).toString();
		QStringRef s10n = attributes.value(QLatin1String("subscription"));
		m_subscription = strToEnum<RosterItem::SubscriptionType>(s10n,subscription_types);
		m_groups.clear();
	} else if (m_depth == 3 && m_state == AtItem && name == QLatin1String("group")) {
		m_state = AtGroup;
	}
}

void AbstractRosterQueryFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	Q_UNUSED(uri);
	Q_UNUSED(name);
	if (m_depth == 1) {
		m_state = AtStart;
	} else if (m_depth == 2) {
		m_state = AtQuery;
		RosterItem::Ptr item = m_roster->createItem();
		RosterItemPrivate *p = RosterItemPrivate::get(item.data());
		p->ask = m_ask;
		p->jid = m_jid;
		p->name = m_name;
		p->groups = m_groups;
		p->subscription = m_subscription;
		m_items << item;
	} else if (m_depth == 3 && m_state == AtGroup) {
		m_state = AtItem;
	}
	m_depth--;
}

void AbstractRosterQueryFactory::handleCharacterData(const QStringRef &text)
{
	if (m_depth == 3 && m_state == AtGroup)
		m_groups << text.toString();
}

void AbstractRosterQueryFactory::serialize(Payload *extension, QXmlStreamWriter *writer)
{
	AbstractRosterQuery *query = se_cast<AbstractRosterQuery*>(extension);
	if (!query)
		return;
	writer->writeStartElement(QLatin1String("query"));
	writer->writeDefaultNamespace(NS_ROSTER);
	if (query->items().isEmpty())
		writer->writeAttribute(QLatin1String("ver"), query->ver());
	foreach (const RosterItem::Ptr &item, query->items()) {
		writer->writeStartElement(QLatin1String("item"));
		writeAttribute(writer,QLatin1String("name"), item->name());
		writer->writeAttribute(QLatin1String("jid"), item->jid());
		writer->writeAttribute(QLatin1String("ask"), item->ask());
		writeAttribute(writer,QLatin1String("subscription"),enumToStr(item->subscription(),subscription_types));
		foreach (const QString &group, item->groups())
			writer->writeTextElement(QLatin1String("group"), group);
		writer->writeEndElement();
	}

	writer->writeEndElement();
}

Payload::Ptr AbstractRosterQueryFactory::createPayload()
{
	return Payload::Ptr(new AbstractRosterQuery(m_items, m_ver));
}

RosterItem::RosterItem(const QString &jid, const QString &name,
                       const QStringList &groups, SubscriptionType s10n)
	: d_ptr(new RosterItemPrivate)
{
	Q_D(RosterItem);
	d->roster = 0;
	d->jid = jid;
	d->name = name;
	d->groups = groups;
	d->subscription = s10n;
}

RosterItem::RosterItem(AbstractRoster *roster, RosterItemPrivate *data)
	: d_ptr(data ? data : new RosterItemPrivate())
{
	Q_D(RosterItem);
	d->roster = roster;
	d->groups.clear();
	d->subscription = Invalid;
}

RosterItem::~RosterItem()
{
}

void RosterItem::setData(const QSharedPointer<RosterItem> &item)
{
	Q_D(RosterItem);
	const RosterItemPrivate *p = item->d_ptr.data();
	d->ask = p->ask;
	d->groups = p->groups;
	d->subscription = p->subscription;
	d->name = p->name;
}

AbstractRoster::AbstractRoster(Client *client, AbstractRosterPrivate *data) : QObject(client), d_ptr(data?data:new AbstractRosterPrivate)
{
	Q_D(AbstractRoster);
	d->client = client;
	d->self = createItem();
	RosterItemPrivate *p = d->self->d_ptr.data();
	p->jid = client->jid().bare();
	p->subscription = RosterItem::Both;
	ClientPrivate::get(client)->roster = this;
	connect(client, SIGNAL(iqReceived(Jreen::IQ)), this, SLOT(handleIQ(Jreen::IQ)));
	//	 connect(client, SIGNAL(presenceReceived(Jreen::Presence)), this, SLOT(handlePresence(Jreen::Presence)));
	init();
}

AbstractRoster::~AbstractRoster()
{
}

QString AbstractRoster::version() const
{
	return d_func()->version;
}

RosterItem::Ptr AbstractRoster::item(const JID &jid) const
{
	return d_func()->items.value(jid.bare());
}

RosterItem::Ptr AbstractRoster::selfItem() const
{
	return d_func()->self;
}

void AbstractRoster::fillRoster(const QString &version, const QList<RosterItem::Ptr> &items)
{
	d_func()->version = version;
	onLoaded(items);
}

void AbstractRoster::load()
{
	Q_D(AbstractRoster);
	IQ iq(IQ::Get, JID(), d->client->getID());
	Logger::debug() << Q_FUNC_INFO << d->version;
	iq.addExtension(new AbstractRosterQuery(d->version));
	d->client->send(iq, this, SLOT(handleIQ(Jreen::IQ,int)), LoadRoster);
}

void AbstractRoster::synchronize()
{
	Q_D(AbstractRoster);
	foreach(const QSharedPointer<RosterItem> &item, d->changed_items) {
		IQ iq(IQ::Set, JID());
		iq.setFrom(d->client->jid());
		iq.addExtension(new AbstractRosterQuery(item));
		d->client->send(iq, this, SLOT(handleIQ(Jreen::IQ,int)), SyncContext);
	}
	d->changed_items.clear();
}

void AbstractRoster::init()
{
	Q_D(AbstractRoster);
	d->client->registerPayload(new AbstractRosterQueryFactory(this));
}

void AbstractRoster::add(const JID &jid, const QString &name, const QStringList &groups)
{
	if(!jid.isValid())
		return;
	Q_D(AbstractRoster);
	QSharedPointer<RosterItem> item(createItem());
	RosterItemPrivate *p = item->d_ptr.data();
	p->jid = jid;
	p->name = name;
	p->groups = groups;
	IQ iq(IQ::Set, JID());
	iq.addExtension(new AbstractRosterQuery(item));
	d->client->send(iq, this, SLOT(handleIQ(Jreen::IQ,int)), AddRosterItem);
}

void AbstractRoster::remove(const JID &jid)
{
	if(!jid.isValid())
		return;
	Q_D(AbstractRoster);
	QSharedPointer<RosterItem> item(createItem());
	item->d_ptr->jid = jid;
	item->d_ptr->subscription = RosterItem::Remove;
	IQ iq(IQ::Set, JID());
	iq.addExtension(new AbstractRosterQuery(item));
	d->client->send(iq, this, SLOT(handleIQ(Jreen::IQ,int)), RemoveRosterItem);
}

QSharedPointer<RosterItem> AbstractRoster::createItem()
{
	return QSharedPointer<RosterItem>(new RosterItem(this));
}

//QSharedPointer<AbstractResource> AbstractRoster::createResource()
//{
//	return QSharedPointer<AbstractResource>(new AbstractResource());
//}

void AbstractRoster::handleIQ(const IQ &iq)
{
	const AbstractRosterQuery::Ptr roster = iq.payload<AbstractRosterQuery>();
	if (!roster)
		return;
	Q_D(AbstractRoster);
	d->version = roster->ver();
	iq.accept();
	foreach (const RosterItem::Ptr &item, roster->items()) {
		Logger::debug() << "handle item" << item->jid();
		if(item->subscription() == RosterItem::Remove) {
			onItemRemoved(item->jid());
			d->items.remove(item->jid());
		} else {
			QHash<QString, RosterItem::Ptr>::iterator item_iter = d->items.find(item->jid());
			if (item_iter == d->items.end()) {
				d->items.insert(item->jid(), item);
				onItemAdded(item);
			} else {
				item_iter.value()->setData(item);
				onItemUpdated(item_iter.value());
			}
		}
	}
}

void AbstractRoster::onItemAdded(QSharedPointer<RosterItem> item)
{
	Q_UNUSED(item);
}

void AbstractRoster::onItemUpdated(QSharedPointer<RosterItem> item)
{
	Q_UNUSED(item);
}

void AbstractRoster::onItemRemoved(const QString &jid)
{
	Q_UNUSED(jid);
}

void AbstractRoster::handleIQ(const IQ &iq, int context)
{
	//Q_D(AbstractRoster);
	switch(context)
	{
	case LoadRoster:
		// By xep-0237 if no newer version of roster
		// presents there is no query element in stanza
		if (AbstractRosterQuery::Ptr roster = iq.payload<AbstractRosterQuery>()) {
			d_func()->version = roster->ver();
			onLoaded(roster->items());
		} else {
			emit loaded();
		}
		break;
	case AddRosterItem:
	case RemoveRosterItem: {
		Logger::debug() << "handle add/remove item" << (iq.subtype() == IQ::Error);
//		IQ request = d->iqHash.take(iq.id());
//		Q_ASSERT(request.subtype() != IQ::Invalid);
//		if(iq.subtype() == IQ::Error)
//			return;
//		handleIQ(request);
//		iq.accept();
		break;
	}
	case SyncContext: {
		//IMPLEMENTME
		break;
	}
	}
}

void AbstractRoster::onLoaded(const QList<QSharedPointer<RosterItem> > &items)
{
	Q_D(AbstractRoster);
	QSet<QString> jidsForRemove;
	QHashIterator<QString, RosterItem::Ptr> it(d->items);
	while (it.hasNext()) {
		it.next();
		jidsForRemove.insert(it.key());
	}
	for (int i = 0; !jidsForRemove.isEmpty() && i < items.size(); i++)
		jidsForRemove.remove(items.at(i)->jid());
	foreach (const RosterItem::Ptr &item, items) {
		QHash<QString, RosterItem::Ptr>::iterator item_iter = d->items.find(item->jid());
		if (item_iter == d->items.end()) {
			d->items.insert(item->jid(), item);
			item->d_func()->roster = this;
			onItemAdded(item);
		} else {
			item_iter.value()->setData(item);
			onItemUpdated(item_iter.value());
		}
	}
	foreach (const QString &jid, jidsForRemove) {
		onItemRemoved(jid);
		d->items.remove(jid);
	}

	emit loaded();
}

QString RosterItem::jid() const
{
	return d_ptr->jid;
}

QString RosterItem::name() const
{
	return d_ptr->name;
}

QStringList RosterItem::groups() const
{
	return d_ptr->groups;
}

RosterItem::SubscriptionType RosterItem::subscription() const
{
	return d_ptr->subscription;
}

QString RosterItem::ask() const
{
	return d_ptr->ask;
}

void RosterItem::setGroups(const QStringList &groups)
{
	setChanged(); d_ptr->groups = groups;
}

void RosterItem::setName(const QString &name)
{
	setChanged(); d_ptr->name = name;
}

void RosterItem::setChanged()
{
	Q_D(RosterItem);
	d->roster->d_func()->changed_items << d->roster->d_func()->items.value(d->jid);
}

//void AbstractRoster::handlePresence(const Presence &presence)
//{
//	QSharedPointer<AbstractRosterItem> item;
//	if(presence.from().bare() == m_self->jid())
//	{
//		item = m_self;
//	}
//	else
//	{
//		QHash<QString, QSharedPointer<AbstractRosterItem> >::iterator item_iter = m_items.find(presence.from().bare());
//		if(item_iter == m_items.end())
//		{
//			emit nonRosterPresence(presence);
//			return;
//		}
//		item = item_iter.value();
//	}
//	if(presence.subtype() == Presence::Unavailable)
//	{
//		item->m_resources.remove(presence.from().resource());
//	}
//	else
//	{
//		QSharedPointer<AbstractResource> resource = item->resource(presence.from().resource());
//		if(!resource)
//		{
//			resource = QSharedPointer<AbstractResource>(createResource());
//			resource->d_ptr->resource = presence.from().resource();
//			item->m_resources.insert(resource->d_ptr->resource, resource);
//		}
//		resource->d_ptr->presence = presence;
//	}
//	if(item == m_self)
//		emit selfPresence(item, presence);
//	else
//		emit rosterPresence(item, presence);
//}

class SimpleRosterPrivate : public AbstractRosterPrivate
{
};

SimpleRoster::SimpleRoster(Client *client, SimpleRosterPrivate *data)
    : AbstractRoster(client, data ? data : new SimpleRosterPrivate)
{
	connect(client, SIGNAL(presenceReceived(Jreen::Presence)),
	        SLOT(onPresenceReceived(Jreen::Presence)));
}

SimpleRoster::~SimpleRoster()
{
}

void SimpleRoster::subscribe(const JID &jid, const QString &msg, const QString &name, const QStringList &groups)
{
	add(jid, name, groups);
	Presence pres(Presence::Subscribe, jid.bareJID(), msg);
	d_func()->client->send(pres);
}

void SimpleRoster::subscribe(const JID &jid, const QString &msg)
{
	Presence pres(Presence::Subscribe, jid.bareJID(), msg);
	d_func()->client->send(pres);
}

void SimpleRoster::unsubscribe(const JID &jid, const QString &msg)
{
	Presence pres(Presence::Unsubscribe, jid.bareJID(), msg);
	d_func()->client->send(pres);
}

void SimpleRoster::allowSubscription(const JID &jid, bool ok)
{
	Presence pres(ok ? Presence::Subscribed : Presence::Unsubscribed, jid.bareJID());
	d_func()->client->send(pres);
}

void SimpleRoster::onPresenceReceived(const Jreen::Presence &presence)
{
	RosterItem::Ptr item = AbstractRoster::item(presence.from());
	if (presence.isSubscription())
		emit subscriptionReceived(item, presence);
	else
		emit presenceReceived(item, presence);
}

void SimpleRoster::onItemAdded(QSharedPointer<RosterItem> item)
{
	emit itemAdded(item);
}

void SimpleRoster::onItemUpdated(QSharedPointer<RosterItem> item)
{
	emit itemUpdated(item);
}

void SimpleRoster::onItemRemoved(const QString &jid)
{
	emit itemRemoved(jid);
}

}
