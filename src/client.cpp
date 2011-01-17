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
#include "capabilitiesfactory_p.h"
#include "errorfactory_p.h"
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
#include "receiptfactory.h"
#include "softwareversionfactory.h"
#include "moodfactory_p.h"
#include "vcardfactory_p.h"
#include "pingfactory_p.h"
#include "vcardupdatefactory_p.h"
#include "ping.h"
#include "privatexml_p.h"
#include "mucroomfactory_p.h"
#include "entitytimefactory_p.h"
#include "pubsubevent.h"
#include "pubsubmanager_p.h"
#include "tunefactory_p.h"
#include "bookmarkfactory.h"
#include "privacyqueryfactory_p.h"

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
		if (MUCRoomPrivate *room = rooms.value(stanza->from().bare()))
			room->handlePresence(*stanza.staticCast<Presence>());
		else
			client->handlePresence(*stanza.staticCast<Presence>());
	}/* else if (type == StanzaPrivate::StanzaSubscription) {
		client->handleSubscription(*stanza.staticCast<Subscription>());
	}*/
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
	CapabilitesFactory *capsFactory = new CapabilitesFactory(disco);

	client->registerStanzaExtension(new ErrorFactory);
	client->registerStanzaExtension(capsFactory);
	client->registerStanzaExtension(new DataFormFactory);
	client->registerStanzaExtension(new DiscoInfoFactory);
	client->registerStanzaExtension(new DiscoItemsFactory);
	client->registerStanzaExtension(new ChatStateFactory);
	client->registerStanzaExtension(new DelayedDeliveryFactory);
	client->registerStanzaExtension(new ReceiptFactory);
	client->registerStanzaExtension(new SoftwareVersionFactory);
	client->registerStanzaExtension(new MoodFactory);
	client->registerStanzaExtension(new TuneFactory);
	client->registerStanzaExtension(new VCardFactory);
	client->registerStanzaExtension(new PingFactory);
	client->registerStanzaExtension(new VCardUpdateFactory);
	client->registerStanzaExtension(new MUCRoomQueryFactory);
	client->registerStanzaExtension(new MUCRoomUserQueryFactory);
	client->registerStanzaExtension(new MUCRoomAdminQueryFactory);
	client->registerStanzaExtension(new MUCRoomOwnerQueryFactory);
	client->registerStanzaExtension(new EntityTimeFactory);
	client->registerStanzaExtension(new BookmarkFactory);
	client->registerStanzaExtension(new PrivateXmlQueryFactory(client));
	client->registerStanzaExtension(new PrivacyQueryFactory);
//	client->registerStanzaExtension(new PubSub::EventFactory);
//	client->registerStanzaExtension(new PubSub::PublishFacatory);
	//client->registerStanzaExtension(new PrivateXml::QueryFactory);

	client->registerStreamFeature(new NonSaslAuth);
	client->registerStreamFeature(new SASLFeature);
	client->registerStreamFeature(new TLSFeature);
	client->registerStreamFeature(new BindFeature);
	client->registerStreamFeature(new SessionFeature);
	client->registerStreamFeature(new ZLibCompressionFeature);
	presence.addExtension(new Capabilities(QString(), QLatin1String("http://jreen.qutim.org/")));
}

Client::Client(const JID &jid, const QString &password, int port)
	: d_ptr(new ClientPrivate(Presence(Presence::Unavailable,JID()), this))
{
	Q_D(Client);
	d->init();
	d->jid = jid;
	d->server = jid.domain();
	d->password = password;
	d->server_port = port;
	d->presence.setFrom(d->jid);
}

Client::Client()
	: d_ptr(new ClientPrivate(Presence(Presence::Unavailable,JID()), this))
{
	d_func()->init();
}

Client::~Client()
{
}

const JID &Client::jid()
{
	return d_func()->jid;
}

void Client::setJID(const JID &jid)
{
	Q_D(Client);
	d->jid = jid;
	d->server = jid.domain();
	d->presence.setFrom(jid);
}

void Client::setPassword(const QString &password)
{
	d_func()->password = password;
}

void Client::setServer(const QString &server)
{
	d_func()->server = server;
}

void Client::setPort(int port)
{
	d_func()->server_port = port;
}

void Client::addXmlStreamHandler(XmlStreamHandler *handler)
{
	d_func()->streamHandlers << handler;
}

QSet<QString> Client::serverFeatures() const
{
	return d_func()->serverFeatures;
}

Disco::IdentityList Client::serverIdentities() const
{
	return d_func()->serverIdentities;
}

void Client::setResource(const QString &resource)
{
	d_func()->jid.setResource(resource);
}

const QString &Client::server() const
{
	return d_func()->server;
}

int Client::port() const
{
	return d_func()->server_port;
}

const QString Client::getID()
{
	return QLatin1Literal("jreen:") % QString::number(qHash(this), 16) % QLatin1Char(':') % QString::number(d_func()->current_id++);
}

Presence &Client::presence()
{
	return d_func()->presence;
}

Disco *Client::disco()
{
	return d_func()->disco;
}

MessageSessionManager *Client::messageSessionManager()
{
	return d_func()->messageSessionManager;
}

AbstractRoster *Client::roster()
{
	return d_func()->roster;
}

void Client::send(const Stanza &stanza)
{
	Q_D(Client);
	if(!d->conn || !d->conn->isOpen())
		return;
	d->send(stanza);
}

void Client::send(const Presence &pres)
{
	Q_D(Client);
	qDebug() << Q_FUNC_INFO << d->jid << d->conn;
	qDebug() << d->conn->isOpen();
	if(!d->conn || !d->conn->isOpen())
		return;
	if (StanzaPrivate::get(pres) == StanzaPrivate::get(d->presence)) {
		d->send(pres);
		return;
	}
	Presence p = pres;
	foreach (const StanzaExtension::Ptr &se, d->presence.extensions())
		p.addExtension(se);
	d->send(p);
}

void Client::send(const IQ &iq, QObject *handler, const char *member, int context)
{
	Q_D(Client);
	if(!d->conn || !d->conn->isOpen())
		return;
	if (iq.id().isEmpty()) {
		const StanzaPrivate *p = StanzaPrivate::get(iq);
		const_cast<StanzaPrivate*>(p)->id = getID();
	}

	qDebug() << "send iq to" << iq.to() << "from" << iq.from();
	QString id = iq.id();
	d->iq_tracks.insert(id, new IQTrack(handler, member, context));
	d->send(iq);
}

void Client::setConnectionImpl(Connection *conn)
{
	Q_D(Client);
	delete d->conn;
	d->conn = conn;
	d->streamProcessor = qobject_cast<StreamProcessor*>(conn);
	d->device->setDevice(conn);
	//	connect(conn, SIGNAL(readyRead()), impl, SLOT(newData()));
	connect(conn, SIGNAL(connected()), d, SLOT(connected()));
	connect(conn, SIGNAL(disconnected()),d, SLOT(disconnected()));
}

Connection *Client::connection() const
{
	return d_func()->conn;
}

void Client::registerStanzaExtension(AbstractStanzaExtensionFactory *factory)
{
	Q_D(Client);
//	delete d->factories.value(factory->extensionType(), 0);
	d->factories.insert(factory->extensionType(), factory);
	foreach (const QString &feature, factory->features()) {
		DiscoPrivate::get(d->disco)->features << feature;
		d->factoriesByUri.insert(feature, factory);
	}
}

inline bool featureLessThan(StreamFeature *a, StreamFeature *b)
{
	return a->type() == b->type() ? a->priority() > b->priority() : a->type() < b->type();
}

void Client::registerStreamFeature(StreamFeature *stream_feature)
{
	Q_D(Client);
	if(!stream_feature)
		return;
	d->features.insert(qLowerBound(d->features.begin(), d->features.end(),
									  stream_feature, featureLessThan), stream_feature);
	stream_feature->setStreamInfo(d->stream_info);
}

void Client::setPingInterval(int interval)
{
	Q_D(Client);
	if (d->pingTimer->isActive())
		d->pingTimer->stop();
	if (interval > 0 && isConnected())
		d->pingTimer->start(interval, this);
}

void Client::setPresence()
{
	send(d_func()->presence);
}

void Client::setPresence(Presence::Type type, const QString &text, int priority)
{
	Q_D(Client);
	if(d->presence.subtype() == type || type == Presence::Error || type == Presence::Invalid || type == Presence::Probe)
		return;
	d->presence.setSubtype(type);
	d->presence.addStatus(text);
	if(priority > -129 && priority < 128)
		d->presence.setPriority(priority);
	setPresence();
}

void Client::connectToServer()
{
	Q_D(Client);
	if(!d->conn)
		setConnectionImpl(new TcpConnection(d->server, d->server_port));

	if(!d->conn->isOpen()) {
		if (d->streamProcessor) {
			d->streamProcessor->setJID(d->jid);
			d->streamProcessor->setStreamParser(d->parser);
		}
		d->conn->open();
	}

}

void Client::disconnectFromServer(bool force)
{
	Q_D(Client);
	if(d->conn && d->conn->isOpen()) {
		setPresence(Presence::Unavailable);
		d->writer->writeEndElement();
		if(force)
			d->conn->close();
	}
}

void ClientPrivate::onIqReceived(const IQ &iq, int context)
{
	Q_UNUSED(context);
	QSharedPointer<Disco::Info> info = iq.findExtension<Disco::Info>();
	if (info) {
		serverFeatures = info->features();
		serverIdentities = info->identities();
		emit client->serverFeaturesReceived(serverFeatures);
		emit client->serverIdentitiesReceived(serverIdentities);
		qDebug() << serverFeatures;
	}
}

void Client::timerEvent(QTimerEvent *timerEvent)
{
}

void Client::handleConnect()
{
	Q_D(Client);
	IQ iq(IQ::Get, d->jid.domain());
	iq.addExtension(new Disco::Info);
	send(iq, d, SLOT(onIqReceived(jreen::IQ,int)), 0);
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
	//handle XMPP::ping
	if(iq.containsExtension<Ping>()) {
		iq.accept();
		IQ pong(IQ::Result,iq.from(),iq.id());
		pong.setFrom(d_func()->jid);
		send(pong); //FIXME remove warning
	}
	if (iq.containsExtension<EntityTime>()) {
		iq.accept();
		IQ result(IQ::Result,iq.from(),iq.id());
		result.addExtension(new EntityTime(QDateTime::currentDateTime()));
		send(result);
	}
	emit newIQ(iq);
}

void Client::handleMessage(const Message &message)
{
	qDebug() << "Handle message" << message.from();
	emit newMessage(message);
}

bool Client::isConnected() const
{
	return d_func()->isConnected;
}

QString Client::password() const
{
	return d_func()->password;
}

}
