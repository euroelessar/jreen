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

#include "privacymanager_p.h"
#include "abstractroster.h"
#include "client.h"
#include "iq.h"
#include "error.h"
#include "logger.h"

namespace Jreen
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

bool PrivacyItem::check(const RosterItem *item) const
{
	const PrivacyItemPrivate *d = d_ptr.data();
	if (d->type == ByJID)
		return check(item->jid());
	if (d->type == ByGroup)
		return item->groups().contains(d->data.toString());
	if (d->type == BySubscription) {
		SubscriptionType itemType = static_cast<SubscriptionType>(item->subscription());
		SubscriptionType type = static_cast<SubscriptionType>(d->data.toInt());
		if (itemType == Invalid)
			itemType = None;
		if (type == Invalid)
			type = None;
		if (itemType == Both && type != None)
			return true;
		return itemType == type;
	}
	return true;
}

bool PrivacyItem::check(const JID &itemJid) const
{
	const PrivacyItemPrivate *d = d_ptr.data();
	if (d->type == All)
		return true;
	if (d->type != ByJID)
		return false;
	JID jid = d->data.value<JID>();
	if (jid.isFull())
		return jid == itemJid;
	if (jid.isDomain())
		return itemJid.domain() == jid.domain();
	if (jid.isBare())
		return itemJid.bare() == jid.bare();
	if (jid.node().isEmpty())
		return itemJid.domain() == jid.domain() && itemJid.resource() == jid.resource();
	return false;
}

PrivacyManager::PrivacyManager(Client *client) : QObject(client), d_ptr(new PrivacyManagerPrivate)
{
	Q_D(PrivacyManager);
	d->client = client;
	d->validServer = true;
	connect(d->client, SIGNAL(iqReceived(Jreen::IQ)), this, SLOT(handleIQ(Jreen::IQ)));
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
	if (!d->validServer) {
		emit listsReceived();
		return;
	}
	IQ iq(IQ::Get, JID(), d->client->getID());
	iq.addExtension(new PrivacyQuery);
	d->client->send(iq, this, SLOT(handleIQ(Jreen::IQ,int)), RequestAll);
	d->lastListName.clear();
	d->lastList.clear();
	d->listRequests.clear();
	d->activeListSetter.clear();
	d->defaultListSetter.clear();
}

QString PrivacyManager::defaultList() const
{
	return d_func()->defaultList;
}

QString PrivacyManager::activeList() const
{
	return d_func()->activeList;
}

void PrivacyManager::desetActiveList()
{
	setActiveList(QLatin1String(""));
}

void PrivacyManager::setActiveList(const QString &name)
{
	Q_D(PrivacyManager);
	if (!d->validServer)
		return;
	IQ iq(IQ::Set, JID(), d->client->getID());
	PrivacyQuery *query = new PrivacyQuery;
	query->activeList = name;
	iq.addExtension(query);
	d->activeListSetter.insert(iq.id(), name);
	d->client->send(iq, this, SLOT(handleIQ(Jreen::IQ,int)), SetActiveList);
}

void PrivacyManager::setDefaultList(const QString &name)
{
	Q_D(PrivacyManager);
	if (!d->validServer)
		return;
	IQ iq(IQ::Set, JID(), d->client->getID());
	PrivacyQuery *query = new PrivacyQuery;
	query->defaultList = name;
	iq.addExtension(query);
	d->defaultListSetter.insert(iq.id(), name);
	d->client->send(iq, this, SLOT(handleIQ(Jreen::IQ,int)), SetDefaultList);
}

void PrivacyManager::setList(const QString &name, const QList<Jreen::PrivacyItem> &list)
{
	Q_D(PrivacyManager);
	if (!d->validServer)
		return;
	IQ iq(IQ::Set, JID(), d->client->getID());
	PrivacyQuery *query = new PrivacyQuery;
	QList<Jreen::PrivacyItem> fixedList = list;
	qint64 lastOrder = -1;
	for (int i = 0; i < fixedList.size(); i++) {
		if (fixedList.at(i).order() == lastOrder)
			fixedList[i].setOrder(lastOrder + 1);
		lastOrder = fixedList.at(i).order();
	}
	query->lists << PrivacyQuery::List(name, list);
	iq.addExtension(query);
	d->client->send(iq, this, SLOT(handleIQ(Jreen::IQ,int)), SetDefaultList);
}

void PrivacyManager::removeList(const QString &name)
{
	setList(name, QList<Jreen::PrivacyItem>());
}

QStringList PrivacyManager::lists() const
{
	return d_func()->lists;
}

void PrivacyManager::requestList(const QString &name)
{
	Q_D(PrivacyManager);
	if (d->lastListName == name) {
		emit listReceived(name, d->lastList);
		return;
	} else if (!d->lists.contains(name) || !d->validServer) {
		emit listReceived(name, QList<Jreen::PrivacyItem>());
		return;
	} else if (d->listRequests.contains(name)) {
		return;
	}
	d->listRequests << name;
	IQ iq(IQ::Get, JID(), d->client->getID());
	PrivacyQuery *query = new PrivacyQuery;
	query->lists << PrivacyQuery::List(name);
	iq.addExtension(query);
	d->client->send(iq, this, SLOT(handleIQ(Jreen::IQ,int)), RequestList);
}

void PrivacyManager::handleIQ(const Jreen::IQ &iq)
{
	PrivacyQuery::Ptr query = iq.payload<PrivacyQuery>();
	if (query && iq.subtype() == IQ::Set) {
		Q_D(PrivacyManager);
		iq.accept();
		IQ res(IQ::Result, JID());
		d->client->send(res);
		for (int i = 0; i < query->lists.size(); i++) {
			const PrivacyQuery::List &list = query->lists.at(i);
			if (list.name == d->lastListName) {
				d->lastListName.clear();
				d->lastList.clear();
			}
			if (!d->lists.contains(list.name))
				d->lists.append(list.name);
			emit listChanged(list.name);
		}
	}
}

void PrivacyManager::handleIQ(const Jreen::IQ &iq, int context)
{
	Q_D(PrivacyManager);
	if (Error::Ptr error = iq.error()) {
		if (error->condition() == Error::ServiceUnavailable)
			d->validServer = false;
	}
	if (context == SetActiveList) {
		QString name = d->activeListSetter.take(iq.id());
		if (iq.subtype() == IQ::Result) {
			d->activeList = name;
			emit activeListChanged(name);
		}
	} else if (context == SetDefaultList) {
		QString name = d->defaultListSetter.take(iq.id());
		if (iq.subtype() == IQ::Result) {
			d->defaultList = name;
			emit defaultListChanged(name);
		}
	}
	PrivacyQuery::Ptr query = iq.payload<PrivacyQuery>();
	if (context == RequestList) {
		if (!query) {
			emit listReceived(QString(), QList<Jreen::PrivacyItem>());
			return;
		}
		const PrivacyQuery::List &list = query->lists.at(0);
		d->lastListName = list.name;
		d->lastList = list.items;
		if (list.items.isEmpty())
			d->lists.removeOne(list.name);
		d->listRequests.remove(list.name);
		emit listReceived(list.name, list.items);
	} else if (context == RequestAll) {
		if (!query) {
			emit listsReceived();
			return;
		}
		QStringList lists;
		for (int i = 0; i < query->lists.size(); i++)
			lists << query->lists.at(i).name;
		d->lists = lists;
		d->defaultList = query->defaultList;
		d->activeList = query->activeList;
		Logger::debug() << Q_FUNC_INFO << d->lists << d->defaultList << d->activeList;
		if (d->activeList.isEmpty())
			d->activeList = d->defaultList;
		emit listsReceived();
	}
}
}
