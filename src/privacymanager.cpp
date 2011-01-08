/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2011 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#include "privacymanager_p.h"
#include "client.h"
#include "iq.h"

namespace jreen
{
PrivacyItem::PrivacyItem() : d_ptr(new PrivacyItemPrivate)
{
}

PrivacyItem::PrivacyItem(const PrivacyItem &o) : d_ptr(o.d_ptr)
{
}

PrivacyItem &PrivacyItem::operator =(const PrivacyItem &o)
{
	d_ptr = o.d_ptr;
	return *this;
}

PrivacyItem::~PrivacyItem()
{
}

PrivacyItem::Type PrivacyItem::type() const
{
	return d_ptr->type;
}

JID PrivacyItem::jid() const
{
	return d_ptr->data.value<JID>();
}

void PrivacyItem::setJID(const JID &jid)
{
	d_ptr->type = ByJID;
	d_ptr->data = qVariantFromValue(jid);
}

PrivacyItem::SubscriptionType PrivacyItem::subscription() const
{
	bool ok;
	SubscriptionType type = static_cast<SubscriptionType>(d_ptr->data.toInt(&ok));
	return ok ? type : Invalid;
}

void PrivacyItem::setSubscription(SubscriptionType type)
{
	d_ptr->type = BySubscription;
	d_ptr->data = int(type);
}

QString PrivacyItem::group() const
{
	return d_ptr->data.toString();
}

void PrivacyItem::setGroup(const QString &group)
{
	d_ptr->type = ByGroup;
	d_ptr->data = group;
}

PrivacyItem::Action PrivacyItem::action() const
{
	return d_ptr->action;
}

void PrivacyItem::setAction(Action action)
{
	d_ptr->action = action;
}

PrivacyItem::StanzaTypes PrivacyItem::stanzaTypes() const
{
	return d_ptr->types;
}

void PrivacyItem::setStanzaTypes(StanzaTypes types)
{
	d_ptr->types = types;
}

uint PrivacyItem::order() const
{
	return d_ptr->order;
}

void PrivacyItem::setOrder(uint order)
{
	d_ptr->order = order;
}

PrivacyManager::PrivacyManager(Client *client) : QObject(client), d_ptr(new PrivacyManagerPrivate)
{
	Q_D(PrivacyManager);
	d->client = client;
}

PrivacyManager::~PrivacyManager()
{
}

enum {
	SetActiveList = 100,
	SetDefaultList,
	SetList,
	RequestList,
	RequestAll
};

void PrivacyManager::request()
{
	Q_D(PrivacyManager);
	IQ iq(IQ::Get, JID(), d->client->getID());
	iq.addExtension(new PrivacyQuery);
	d->client->send(iq, this, SLOT(handleIQ(jreen::IQ,int)), RequestAll);
}

QString PrivacyManager::defaultList() const
{
	return d_func()->defaultList;
}

QString PrivacyManager::activeList() const
{
	return d_func()->activeList;
}

void PrivacyManager::setActiveList(const QString &name)
{
	Q_D(PrivacyManager);
	IQ iq(IQ::Set, JID(), d->client->getID());
	PrivacyQuery *query = new PrivacyQuery;
	query->activeList = name;
	iq.addExtension(query);
	d->client->send(iq, this, SLOT(handleIQ(jreen::IQ,int)), SetActiveList);
}

void PrivacyManager::setDefaultList(const QString &name)
{
	Q_D(PrivacyManager);
	IQ iq(IQ::Set, JID(), d->client->getID());
	PrivacyQuery *query = new PrivacyQuery;
	query->defaultList = name;
	iq.addExtension(query);
	d->client->send(iq, this, SLOT(handleIQ(jreen::IQ,int)), SetDefaultList);
}

void PrivacyManager::setList(const QString &name, const QList<jreen::PrivacyItem> &list)
{
	Q_D(PrivacyManager);
	IQ iq(IQ::Set, JID(), d->client->getID());
	PrivacyQuery *query = new PrivacyQuery;
	query->lists << PrivacyQuery::List(name, list);
	iq.addExtension(query);
	d->client->send(iq, this, SLOT(handleIQ(jreen::IQ,int)), SetDefaultList);
}

void PrivacyManager::removeList(const QString &name)
{
	setList(name, QList<jreen::PrivacyItem>());
}

QStringList PrivacyManager::lists() const
{
	return d_func()->lists;
}

void PrivacyManager::requestList(const QString &name)
{
	Q_D(PrivacyManager);
	IQ iq(IQ::Get, JID(), d->client->getID());
	PrivacyQuery *query = new PrivacyQuery;
	query->lists << PrivacyQuery::List(name);
	iq.addExtension(query);
	d->client->send(iq, this, SLOT(handleIQ(jreen::IQ,int)), RequestList);
}

void PrivacyManager::handleIQ(const jreen::IQ &iq)
{
	PrivacyQuery::Ptr query = iq.findExtension<PrivacyQuery>();
	if (query && iq.subtype() == IQ::Set) {
		Q_D(PrivacyManager);
		iq.accept();
		IQ res(IQ::Result, JID());
		d->client->send(res);
		for (int i = 0; i < query->lists.size(); i++)
			emit listChanged(query->lists.at(i).name);
	}
}

void PrivacyManager::handleIQ(const jreen::IQ &iq, int context)
{
	PrivacyQuery::Ptr query = iq.findExtension<PrivacyQuery>();
	if (!query)
		return;
	Q_D(PrivacyManager);
	if (context == RequestList && !query->lists.isEmpty()) {
		const PrivacyQuery::List &list = query->lists.at(0);
		emit listReceived(list.name, list.items);
	} else if (context == RequestAll) {
		QStringList lists;
		for (int i = 0; i < query->lists.size(); i++)
			lists << query->lists.at(i).name;
		d->lists = lists;
		d->defaultList = query->defaultList;
		d->activeList = query->activeList;
		if (d->activeList.isEmpty())
			d->activeList = d->defaultList;
		emit listsReceived();
	}
}
}
