/****************************************************************************
 *  abstractroster.cpp
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#include "abstractroster_p.h"
#include "client.h"
#include "jstrings.h"
#include "iq.h"
#include "jid.h"
#include <QXmlStreamWriter>

namespace jreen
{
	AbstractRosterQueryFactory::AbstractRosterQueryFactory(AbstractRoster *roster)
	{
		m_roster = roster;
		m_state = AtStart;
		m_depth = 0;
	}

	QStringList AbstractRosterQueryFactory::features() const
	{
		return QStringList(QLatin1String("jabber:iq:roster"));
	}
	
	bool AbstractRosterQueryFactory::canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
	{
		Q_UNUSED(attributes);
		return name == QLatin1String("query") && uri == QLatin1String("jabber:iq:roster");
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
			QStringRef s10n = attributes.value(QLatin1String("subscription"));
			if (s10n == QLatin1String("to"))
				m_subscription = AbstractRosterItem::To;
			else if (s10n == QLatin1String("from"))
				m_subscription = AbstractRosterItem::From;
			else if (s10n == QLatin1String("both"))
				m_subscription = AbstractRosterItem::Both;
			else if (s10n == QLatin1String("remove"))
				m_subscription = AbstractRosterItem::Remove;
			else
				m_subscription = AbstractRosterItem::None;
			m_groups.clear();
		} else if (m_depth == 3 && m_state == AtItem && name == QLatin1String("group")) {
			m_state = AtGroup;
		}
	}

	void AbstractRosterQueryFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
	{
		if (m_depth == 1) {
			m_state = AtStart;
		} else if (m_depth == 2) {
			m_state = AtQuery;
			AbstractRosterItem::Ptr item = m_roster->createItem();
			AbstractRosterItemPrivate *p = AbstractRosterItemPrivate::get(item.data());
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

	void AbstractRosterQueryFactory::serialize(StanzaExtension *extension, QXmlStreamWriter *writer)
	{
		AbstractRosterQuery *query = se_cast<AbstractRosterQuery*>(extension);
		if (!query)
			return;
		writer->writeStartElement(QLatin1String("query"));
		writer->writeDefaultNamespace(QLatin1String("jabber:iq:roster"));
		if (m_items.isEmpty())
			writer->writeAttribute(QLatin1String("ver"), QLatin1String(""));
		foreach (const AbstractRosterItem::Ptr &item, m_items) {
			writer->writeStartElement(QLatin1String("item"));
			writer->writeAttribute(QLatin1String("name"), item->name());
			writer->writeAttribute(QLatin1String("jid"), item->jid());
			foreach (const QString &group, item->groups())
				writer->writeTextElement(QLatin1String("group"), group);
			writer->writeEndElement();
		}

		writer->writeEndElement();
	}

	StanzaExtension::Ptr AbstractRosterQueryFactory::createExtension()
	{
		return StanzaExtension::Ptr(new AbstractRosterQuery(m_items));
	}

J_STRING(group)
J_STRING(subscription)
J_STRING(ask)

static const QStringList roster_subscriptions = QStringList()
												<< ConstString::from << ConstString::to
												<< QLatin1String("both") << QLatin1String("remove");

AbstractRosterItem::AbstractRosterItem(const QDomElement &node, AbstractRoster *roster, AbstractRosterItemPrivate *data)
	: d_ptr(data ? data : new AbstractRosterItemPrivate())
{
	Q_D(AbstractRosterItem);
	d->roster = roster;
	if(node.isNull())
		return;
	d->groups.clear();
	forelements(const QDomElement &elem, node)
	{
		if(elem.nodeName() == group_str)
		{
			d->groups << elem.text();
		}
	}
	d->ask = node.attribute(ask_str);
	int sub = roster_subscriptions.indexOf(node.attribute(subscription_str));
	d->subscription = sub == -1 ? None : static_cast<SubscriptionType>(sub);
	d->jid = node.attribute(ConstString::jid);
	d->name = node.attribute(ConstString::name);
}

AbstractRosterItem::~AbstractRosterItem()
{
}

QDomElement AbstractRosterItem::node(QDomDocument *doc) const
{
	Q_D(const AbstractRosterItem);
	QDomElement node; /*= createElement(doc, ConstString::item);
	node.setAttribute(ConstString::jid, d->jid);
	node.setAttribute(subscription_str, d->subscription == None ? QString() : roster_subscriptions[d->subscription]);
	if(d->subscription != Remove)
	{
		if(!d->ask.isEmpty())
			node.setAttribute(ask_str, d->ask);
		if(!d->name.isEmpty())
			node.setAttribute(ConstString::name, d->name);
		foreach(const QString &group, d->groups)
		{
			node.appendChild(createElement(doc, group_str, group));
		}
	}*/
	return node;
}

void AbstractRosterItem::setData(const QSharedPointer<AbstractRosterItem> &item)
{
	Q_D(AbstractRosterItem);
	const AbstractRosterItemPrivate *p = item->d_ptr.data();
	d->ask = p->ask;
	d->groups = p->groups;
	d->subscription = p->subscription;
	d->name = p->name;
}

 AbstractRoster::AbstractRoster(Client *client, AbstractRosterPrivate *data) : QObject(client), d_ptr(data?data:new AbstractRosterPrivate)
 {
	 d_ptr->client = client;
	 m_self = createItem();
	 AbstractRosterItemPrivate *d = m_self->d_ptr.data();
	 d->jid = client->jid().bare();
	 d->subscription = AbstractRosterItem::Both;
	 connect(client, SIGNAL(newPresence(Presence)), this, SLOT(handlePresence(Presence)));
	 init();
 }

AbstractRoster::~AbstractRoster()
{
}

void AbstractRoster::load()
{
	Q_D(AbstractRoster);
	IQ iq(IQ::Get, JID(), d->client->getID());
	iq.addExtension(new AbstractRosterQuery);
	d->client->send(iq, this, SLOT(handleIQ(IQ,int)), LoadRoster);
}

void AbstractRoster::synchronize()
{
	Q_D(AbstractRoster);
	foreach(const QSharedPointer<AbstractRosterItem> &item, m_changed_items)
	{
		IQ iq(IQ::Set, JID());
		iq.addExtension(new AbstractRosterQuery(item));
		d->client->send(iq, this, SLOT(handleIQ(IQ,int)), SyncContext);
	}
	m_changed_items.clear();
}

void AbstractRoster::init()
{
	Q_D(AbstractRoster);
	d->client->registerStanzaExtension(new AbstractRosterQueryFactory(this));
}

void AbstractRoster::add(const JID &jid, const QString &name, const QStringList &groups)
{
	if(!jid.isValid())
		return;
	Q_D(AbstractRoster);
	QSharedPointer<AbstractRosterItem> item(createItem());
	AbstractRosterItemPrivate *p = item->d_ptr.data();
	p->jid = jid;
	p->subscription = AbstractRosterItem::None;
	p->name = name;
	p->groups = groups;
	IQ iq(IQ::Set, JID());
	iq.addExtension(new AbstractRosterQuery(item));
	d->client->send(iq, this, SLOT(handleIQ(IQ,int)), AddRosterItem);
}

void AbstractRoster::remove(const JID &jid)
{
	if(!jid.isValid())
		return;
	Q_D(AbstractRoster);
	QSharedPointer<AbstractRosterItem> item(createItem());
	item->d_ptr->jid = jid;
	IQ iq(IQ::Set, JID());
	iq.addExtension(new AbstractRosterQuery(item));
	d->client->send(iq, this, SLOT(handleIQ(IQ,int)), AddRosterItem);
}

QSharedPointer<AbstractRosterItem> AbstractRoster::createItem(const QDomElement &node)
{
	return QSharedPointer<AbstractRosterItem>(new AbstractRosterItem(node, this));
}

//QSharedPointer<AbstractResource> AbstractRoster::createResource()
//{
//	return QSharedPointer<AbstractResource>(new AbstractResource());
//}

void AbstractRoster::handleIQ(const IQ &iq)
{
	const AbstractRosterQuery *roster = iq.findExtension<AbstractRosterQuery>().data();
	if(!roster)
		return;
	foreach(const QSharedPointer<AbstractRosterItem> &item, roster->items())
	{
		if(item->subscriptionType() == AbstractRosterItem::Remove)
		{
			m_items.remove(item->jid());
			onItemRemoved(item->jid());
		}
		else
		{
			QHash<QString, QSharedPointer<AbstractRosterItem> >::iterator item_iter = m_items.find(iq.from().bare());
			if(item_iter == m_items.end())
			{
				m_items.insert(item_iter.value()->jid(), item_iter.value());
				onItemAdded(item_iter.value());
			}
			else
			{
				item_iter.value()->setData(item);
				onItemUpdated(item_iter.value());
			}
		}
	}
}

void AbstractRoster::handleIQ(const IQ &iq, int context)
{
	switch(context)
	{
	case LoadRoster:
		// By xep-0237 if tno newer version of roster
		// presents there is no query element in stanza
		if (AbstractRosterQuery::Ptr roster = iq.findExtension<AbstractRosterQuery>()) {
			m_items.clear();
			onLoaded(roster->items());
		}
		break;
	default:
		break;
	}
}

void AbstractRoster::onLoaded(const QList<QSharedPointer<AbstractRosterItem> > &items)
{
	foreach(const QSharedPointer<AbstractRosterItem> &item, items)
	{
		m_items.insert(item->jid(), item);
	}
	emit loaded();
}

const QString &AbstractRosterItem::jid() const
{
	return d_ptr->jid;
}

const QString &AbstractRosterItem::name() const
{
	return d_ptr->name;
}

const QStringList &AbstractRosterItem::groups() const
{
	return d_ptr->groups;
}

AbstractRosterItem::SubscriptionType AbstractRosterItem::subscriptionType() const
{
	return d_ptr->subscription;
}

const QString &AbstractRosterItem::ask() const
{
	return d_ptr->ask;
}

void AbstractRosterItem::setGroups(const QStringList &groups)
{
	setChanged(); d_ptr->groups = groups;
}

void AbstractRosterItem::setName(const QString &name)
{
	setChanged(); d_ptr->name = name;
}

void AbstractRosterItem::setChanged()
{
	Q_D(AbstractRosterItem);
	d->roster->m_changed_items << d->roster->m_items.value(d->jid);
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

}
