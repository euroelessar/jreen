/****************************************************************************
**
** Jreen
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "metacontactstorage.h"
#include "metacontacts_p.h"
#include "logger.h"
#include "client.h"

namespace Jreen {

class MetaContactStorage::ItemData : public QSharedData
{
public:
	ItemData() : order(-1) {}
	ItemData(const ItemData &o)
	    : QSharedData(o), jid(o.jid), tag(o.tag), order(o.order) {}

	JID jid;
	QString tag;
	qint64 order;
};

MetaContactStorage::Item::Item() : d(new ItemData)
{
}

MetaContactStorage::Item::Item(const JID &jid, const QString &tag) : d(new MetaContactStorage::ItemData)
{
	d->jid = jid;
	d->tag = tag;
}

MetaContactStorage::Item::Item(const JID &jid, const QString &tag, uint order) : d(new MetaContactStorage::ItemData)
{
	d->jid = jid;
	d->tag = tag;
	d->order = order;
}

MetaContactStorage::Item::Item(const MetaContactStorage::Item &item) : d(item.d)
{
}

MetaContactStorage::Item &MetaContactStorage::Item::operator =(const MetaContactStorage::Item &item)
{
	d = item.d;
	return *this;
}

MetaContactStorage::Item::~Item()
{
}

JID MetaContactStorage::Item::jid() const
{
	return d->jid;
}

void MetaContactStorage::Item::setJID(const JID &jid)
{
	d->jid = jid;
}

QString MetaContactStorage::Item::tag() const
{
	return d->tag;
}

void MetaContactStorage::Item::setTag(const QString &tag)
{
	d->tag = tag;
}

uint MetaContactStorage::Item::order() const
{
	return uint(d->order);
}

bool MetaContactStorage::Item::hasOrder() const
{
	return d->order >= 0;
}

void MetaContactStorage::Item::clearOrder()
{
	d->order = -1;
}

void MetaContactStorage::Item::setOrder(uint order)
{
	d->order = order;
}

class MetaContactStoragePrivate
{
public:
	Client *client;
	QWeakPointer<PrivateXml> privateXml;
};

MetaContactStorage::MetaContactStorage(Client *client) :
    QObject(client), d_ptr(new MetaContactStoragePrivate)
{
	Q_D(MetaContactStorage);
	d->client = client;
}

MetaContactStorage::~MetaContactStorage()
{
}

void MetaContactStorage::setPrivateXml(PrivateXml *privateXml)
{
	d_func()->privateXml = privateXml;
}

void MetaContactStorage::requestMetaContacts()
{
	Q_D(MetaContactStorage);
	if (!d->privateXml)
		return;
	d->privateXml.data()->request(QLatin1String("storage"), QLatin1String("storage:metacontacts"), this,
	                              SLOT(onResultReady(Jreen::Payload::Ptr,Jreen::PrivateXml::Result,Jreen::Error::Ptr)));
}

void MetaContactStorage::storeMetaContacts(const MetaContactStorage::ItemList &items)
{
	Q_D(MetaContactStorage);
	if (!d->privateXml)
		return;
	d->privateXml.data()->store(MetaContacts::Ptr(new MetaContacts(items)), this,
	                            SLOT(onResultReady(Jreen::Payload::Ptr,Jreen::PrivateXml::Result,Jreen::Error::Ptr)));
}

void MetaContactStorage::onResultReady(const Payload::Ptr &payload, PrivateXml::Result result, const Error::Ptr &error)
{
	Q_UNUSED(error);
	Logger::debug() << "onResultReady";
	if(result == PrivateXml::RequestOk) {
		MetaContacts *metacontacts = payload_cast<MetaContacts*>(payload.data());
		Logger::debug() << "received metacontacts" << metacontacts << payload.data();
		if (metacontacts)
			emit metaContactsReceived(metacontacts->items);
		else
			emit metaContactsReceived(ItemList());
	}
}

} // namespace Jreen
