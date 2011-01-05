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

#include "pubsubmanager_p.h"
#include "pubsubeventfactory_p.h"
#include "pubsubpublishfactory_p.h"
#include "client_p.h"

namespace jreen
{
	namespace PubSub
	{
		Manager::Manager(Client *client) : QObject(client), d_ptr(new ManagerPrivate)
		{
			Q_D(Manager);
			d->client = client;
			d->client->registerStanzaExtension(new EventFactory(d->factories));
			d->client->registerStanzaExtension(new PublishFactory(d->factories));
			connect(d->client, SIGNAL(newMessage(jreen::Message)),
					this, SLOT(handleMessage(jreen::Message)));
		}

		Manager::~Manager()
		{
		}
		
		void Manager::publishItems(const QList<StanzaExtension::Ptr> &items, const JID &to)
		{
			IQ iq(IQ::Set, to);
			iq.addExtension(new Publish(items));
			d_func()->client->send(iq);
		}

		void Manager::addEntityType(int type)
		{
			Q_D(Manager);
			ClientPrivate *c = ClientPrivate::get(d->client);
			AbstractStanzaExtensionFactory *factory = c->factories.value(type);
			Q_ASSERT(factory);
			d->factories.append(factory);
			QString node = factory->features().value(0);
			Q_ASSERT(!node.isEmpty());
			d->client->disco()->addFeature(node);
			d->client->disco()->addFeature(node + QLatin1String("+notify"));
			qDebug("%s %d %s", Q_FUNC_INFO, d->factories.size(), qPrintable(node));
		}
		
		void Manager::handleMessage(const jreen::Message &message)
		{
			if (Event::Ptr event = message.findExtension<Event>())
				emit eventReceived(event, message.from());
		}
	}
}
