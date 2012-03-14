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

#include "pubsubmanager_p.h"
#include "pubsubeventfactory_p.h"
#include "pubsubpublishfactory_p.h"
#include "client_p.h"
#include "logger.h"

namespace Jreen
{
namespace PubSub
{
class PublishOptionsPrivate : public QSharedData
{
public:
	PublishOptionsPrivate() : accessModel(PublishOptions::PresenceAccess), persistent(true) {}
	PublishOptionsPrivate(PublishOptionsPrivate &o)
		: QSharedData(o), accessModel(o.accessModel), persistent(o.persistent) {}
	PublishOptions::AccessModel accessModel;
	bool persistent;
};

PublishOptions::PublishOptions() : d_ptr(new PublishOptionsPrivate)
{
	d_ptr->accessModel = PresenceAccess;
	d_ptr->persistent = true;
}

PublishOptions::PublishOptions(const PublishOptions &o) : d_ptr(o.d_ptr)
{
}

PublishOptions &PublishOptions::operator =(const PublishOptions &o)
{
	d_ptr = o.d_ptr;
	return *this;
}

PublishOptions::~PublishOptions()
{
}

PublishOptions::AccessModel PublishOptions::accessModel() const
{
	return d_ptr->accessModel;
}

void PublishOptions::setAccessModel(AccessModel model)
{
	d_ptr->accessModel = model;
}

bool PublishOptions::isPersistent() const
{
	return d_ptr->persistent;
}

void PublishOptions::setPersistent(bool persistent)
{
	d_ptr->persistent = persistent;
}

Manager::Manager(Client *client) : QObject(client), d_ptr(new ManagerPrivate)
{
	Q_D(Manager);
	d->client = client;
	d->client->registerPayload(new EventFactory(d->factories));
	d->client->registerPayload(new PublishFactory(d->factories));
	connect(d->client, SIGNAL(messageReceived(Jreen::Message)),
			this, SLOT(handleMessage(Jreen::Message)));
}

Manager::~Manager()
{
}

static const char *access_strs[] = {
	"authorize",
	"open",
	"presence",
	"roster",
	"whitelist"
};

void Manager::publishItems(const QList<Payload::Ptr> &items, const JID &to)
{
	IQ iq(IQ::Set, to);
	iq.addExtension(new Publish(items, DataForm::Ptr()));
	d_func()->client->send(iq);
}

void Manager::publishItems(const QList<Payload::Ptr> &items, const JID &to,
						   const PublishOptions &options)
{
	IQ iq(IQ::Set, to);
	DataForm::Ptr form(new DataForm(DataForm::Submit));
	form->setTypeName(QLatin1String("http://jabber.org/protocol/pubsub#publish-options"));
	form->appendField(DataFormFieldNone(QLatin1String("pubsub#access_model"),
										QStringList(enumToStr(options.accessModel(), access_strs))));
	form->appendField(DataFormFieldBoolean(QLatin1String("pubsub#persist_items"), true));
	iq.addExtension(new Publish(items, form));
	d_func()->client->send(iq);
}

void Manager::addEntityType(int type)
{
	Q_D(Manager);
	ClientPrivate *c = ClientPrivate::get(d->client);
	AbstractPayloadFactory *factory = c->factories.value(type);
	Q_ASSERT(factory);
	d->factories.append(factory);
	QString node = factory->features().value(0);
	Q_ASSERT(!node.isEmpty());
	d->client->disco()->addFeature(node);
	d->client->disco()->addFeature(node + QLatin1String("+notify"));
	Logger::debug() << Q_FUNC_INFO << d->factories.size() << node;
}

void Manager::handleMessage(const Jreen::Message &message)
{
	if (Event::Ptr event = message.payload<Event>())
		emit eventReceived(event, message.from());
}
}
}
