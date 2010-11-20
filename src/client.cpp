/****************************************************************************
 *  client.cpp
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
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

#include "client_p.h"
#include "disco_p.h"
#include "stanza_p.h"
#include "tcpconnection.h"
#include "nonsaslauth.h"
#include "delayeddelivery.h"
#include "chatstatefactory_p.h"
#include "capabilities.h"
#include "error.h"
#include "dataform.h"
#include "iqfactory_p.h"
#include "presencefactory_p.h"
#include "messagefactory_p.h"
#include "saslfeature.h"
#include "tlsfeature.h"
#include "bindfeature.h"
#include "sessionfeature_p.h"
#include "zlibcompressionfeature.h"
#include <QStringBuilder>
#include "delayeddeliveryfactory.h"

namespace jreen
{
enum State
{
	WaitingForStanza,
	ReadFeatures,
	ReadStanza,
	ReadCustom
};

void ClientPrivate::handleStanza(const Stanza::Ptr &stanza)
{
	if (!stanza)
		return;
	int type = StanzaPrivate::get(*stanza)->type;
	if (type == StanzaPrivate::StanzaIq) {
		QSharedPointer<IQ> iq = stanza.staticCast<IQ>();
		IQTrack *track = iq_tracks.take(stanza->id());
		if (track) {
			emit track->newIQ(*iq, track->context);
			delete track;
		} else {
			client->handleIQ(*iq);
			if (!iq->accepted() && (iq->subtype() == IQ::Set || iq->subtype() == IQ::Get)) {
				IQ error(IQ::Error, iq->from(), iq->id());
				error.addExtension(new Error(Error::Cancel, Error::ServiceUnavailable));
				send(error);
			}
		}
	} else if (type == StanzaPrivate::StanzaMessage) {
		client->handleMessage(*stanza.staticCast<Message>());
	} else if (type == StanzaPrivate::StanzaPresence) {
		client->handlePresence(*stanza.staticCast<Presence>());
	} else if (type == StanzaPrivate::StanzaSubscription) {
		client->handleSubscription(*stanza.staticCast<Subscription>());
	}
}

void ClientPrivate::readMore()
{
}

void ClientPrivate::init()
{
	parser = new Parser(client);
	stanzas << new IqFactory(client);
	stanzas << new PresenceFactory(client);
	stanzas << new MessageFactory(client);
	stream_info = new StreamInfoImpl(this);
	disco = new Disco(client);

	client->registerStanzaExtension(new Error);
	client->registerStanzaExtension(new Capabilities);
	client->registerStanzaExtension(new DataFormFactory);
	client->registerStanzaExtension(new DiscoInfoFactory);
	client->registerStanzaExtension(new Disco::Items);
	client->registerStanzaExtension(new ChatStateFactory);
	client->registerStanzaExtension(new DelayedDeliveryFactory);

	client->registerStreamFeature(new NonSaslAuth);
	client->registerStreamFeature(new SASLFeature);
	client->registerStreamFeature(new TLSFeature);
	client->registerStreamFeature(new BindFeature);
	client->registerStreamFeature(new SessionFeature);
	client->registerStreamFeature(new ZLibCompressionFeature);
	presence.addExtension(new Capabilities(disco));
}

Client::Client(const JID &jid, const QString &password, int port)
	: impl(new ClientPrivate(Presence(Presence::Unavailable,JID()), this))
{
	impl->init();
	impl->jid = jid;
	impl->server = jid.domain();
	impl->password = password;
	impl->server_port = port;
}

Client::Client()
	: impl(new ClientPrivate(Presence(Presence::Unavailable,JID()), this))
{
	impl->init();
}

Client::~Client()
{
	delete impl;
}

const JID &Client::jid()
{
	return impl->jid;
}

void Client::setJID(const JID &jid)
{
	impl->jid = jid;
	impl->server = jid.domain();
}

void Client::setPassword(const QString &password)
{
	impl->password = password;
}

void Client::setServer(const QString &server)
{
	impl->server = server;
}

void Client::setPort(int port)
{
	impl->server_port = port;
}

QSet<QString> Client::serverFeatures() const
{
	return impl->serverFeatures;
}

void Client::setResource(const QString &resource)
{
	impl->jid.setResource(resource);
}

const QString &Client::server() const
{
	return impl->server;
}

int Client::port() const
{
	return impl->server_port;
}

const QString Client::getID()
{
	return QLatin1Literal("jreen:") % QString::number(qHash(this), 16) % QLatin1Char(':') % QString::number(impl->current_id++);
}

Presence &Client::presence()
{
	return impl->presence;
}

Disco *Client::disco()
{
	return impl->disco;
}

void Client::send(const Stanza &stanza)
{
	if(!impl->conn || !impl->conn->isOpen())
		return;
	impl->send(stanza);
}

void Client::send(const IQ &iq, QObject *handler, const char *member, int context)
{
	if(!impl->conn || !impl->conn->isOpen())
		return;
	if (iq.id().isEmpty()) {
		const StanzaPrivate *p = StanzaPrivate::get(iq);
		const_cast<StanzaPrivate*>(p)->id = getID();
	}
	qDebug() << iq.to();
	QString id = iq.id();
	impl->iq_tracks.insert(id, new IQTrack(handler, member, context));
	impl->send(iq);
}

void Client::setConnectionImpl(Connection *conn)
{
	delete impl->conn;
	impl->conn = conn;
	impl->device->setDevice(conn);
	//	connect(conn, SIGNAL(readyRead()), impl, SLOT(newData()));
	connect(conn, SIGNAL(connected()), impl, SLOT(connected()));
	connect(conn, SIGNAL(disconnected()), impl, SLOT(disconnected()));
}

void Client::registerStanzaExtension(StanzaExtension *stanza_extension)
{
	impl->xquery.registerStanzaExtension(stanza_extension, impl->disco);
}

void Client::registerStanzaExtension(AbstractStanzaExtensionFactory *factory)
{
	delete impl->factories.value(factory->extensionType(), 0);
	impl->factories.insert(factory->extensionType(), factory);
	foreach (const QString &feature, factory->features())
		DiscoPrivate::get(impl->disco)->features << feature;
}

inline bool featureLessThan(StreamFeature *a, StreamFeature *b)
{
	return a->type() == b->type() ? a->priority() > b->priority() : a->type() < b->type();
}

void Client::registerStreamFeature(StreamFeature *stream_feature)
{
	if(!stream_feature)
		return;
	impl->features.insert(qLowerBound(impl->features.begin(), impl->features.end(),
									  stream_feature, featureLessThan), stream_feature);
	stream_feature->setStreamInfo(impl->stream_info);
}

void Client::whitespacePing(int period)
{
	impl->send(QLatin1String(" "));
	if(!impl)
		QTimer::singleShot(period, this, SLOT(whitespacePing()));
}

void Client::setPresence()
{
	send(impl->presence);
}

void Client::setPresence(Presence::Type type, const QString &text, int priority)
{
	if(impl->presence.subtype() == type || type == Presence::Error || type == Presence::Invalid || type == Presence::Probe)
		return;
	impl->presence.setPresence(type);
	impl->presence.addStatus(text);
	if(priority > -129 && priority < 128)
		impl->presence.setPriority(priority);
	setPresence();
}

void Client::connectToServer()
{
	if(!impl->conn)
		setConnectionImpl(new TcpConnection(impl->server, impl->server_port));

	if(!impl->conn->isOpen())
		impl->conn->open();

}

void Client::disconnectFromServer(bool force)
{
	if(impl->conn && impl->conn->isOpen())
	{
		impl->presence.setPresence(Presence::Unavailable);
		setPresence();
		if(force)
			impl->conn->close();
	}
}

void ClientPrivate::onIqReceived(const IQ &iq, int context)
{
	Q_UNUSED(context);
	QSharedPointer<Disco::Info> info = iq.findExtension<Disco::Info>();
	if (info) {
		serverFeatures = info->features();
		emit client->serverFeaturesReceived(serverFeatures);
		qDebug() << serverFeatures;
	}
}

void Client::handleConnect()
{
	IQ iq(IQ::Get, impl->jid.domain());
	iq.addExtension(new Disco::Info);
	send(iq, impl, SLOT(onIqReceived(jreen::IQ,int)), 0);
	emit connected();
}

void Client::handleDisconnect()
{
	emit disconnected();
}

void Client::handleAuthorized()
{
	emit authorized();
}

void Client::handleSubscription(const Subscription &subscription)
{
	emit newSubscription(subscription);
}

void Client::handlePresence(const Presence &presence)
{
	emit newPresence(presence);
}

void Client::handleIQ(const IQ &iq)
{
	emit newIQ(iq);
}

void Client::handleMessage(const Message &message)
{
	qDebug() << "Handle message" << message.from();
	emit newMessage(message);
}
}
