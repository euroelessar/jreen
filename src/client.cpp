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
#include "abstractroster.h"
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
#include "activityfactory_p.h"
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

namespace Jreen
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
			bool ok = jid.isDomain() || !roster || rooms.contains(iq->from().bare()) || iq->from().bare() == jid.bare();
			if (!ok) {
				RosterItem::Ptr item = roster->item(iq->from());
				ok = item
						&& item->subscription() != RosterItem::None
						&& item->subscription() != RosterItem::To;
			}
			if (!ok) {
				IQ error(IQ::Error, iq->from(), iq->id());
				foreach (const StanzaExtension::Ptr &se, iq->extensions())
					error.addExtension(se);
				error.addExtension(new Error(Error::Cancel, Error::SubscriptionRequired));
				send(error);
				return;
			}
			q_ptr->handleIQ(*iq);
			if (!iq->accepted() && (iq->subtype() == IQ::Set || iq->subtype() == IQ::Get)) {
				IQ error(IQ::Error, iq->from(), iq->id());
				error.addExtension(new Error(Error::Cancel, Error::ServiceUnavailable));
				send(error);
			}
		}
	} else if (type == StanzaPrivate::StanzaMessage) {
		q_ptr->handleMessage(*stanza.staticCast<Message>());
	} else if (type == StanzaPrivate::StanzaPresence) {
		if (MUCRoomPrivate *room = rooms.value(stanza->from().bare()))
			room->handlePresence(*stanza.staticCast<Presence>());
		else
			q_ptr->handlePresence(*stanza.staticCast<Presence>());
	}/* else if (type == StanzaPrivate::StanzaSubscription) {
		client->handleSubscription(*stanza.staticCast<Subscription>());
	}*/
}

void ClientPrivate::_q_read_more()
{
}

void ClientPrivate::init()
{
	parser = new Parser(q_ptr);
	stanzas << new IqFactory(q_ptr);
	stanzas << new PresenceFactory(q_ptr);
	stanzas << new MessageFactory(q_ptr);
	stream_info = new StreamInfoImpl(this);
	disco = new Disco(q_ptr);
	CapabilitesFactory *capsFactory = new CapabilitesFactory(disco);

	q_ptr->registerStanzaExtension(new ErrorFactory);
	q_ptr->registerStanzaExtension(capsFactory);
	q_ptr->registerStanzaExtension(new DataFormFactory);
	q_ptr->registerStanzaExtension(new DiscoInfoFactory);
	q_ptr->registerStanzaExtension(new DiscoItemsFactory);
	q_ptr->registerStanzaExtension(new ChatStateFactory);
	q_ptr->registerStanzaExtension(new DelayedDeliveryFactory);
	q_ptr->registerStanzaExtension(new ReceiptFactory);
	q_ptr->registerStanzaExtension(new SoftwareVersionFactory);
	q_ptr->registerStanzaExtension(new MoodFactory);
	q_ptr->registerStanzaExtension(new TuneFactory);
	//q_ptr->registerStanzaExtension(new ActivityFactory); Sokol, try to add activitypep converter
	q_ptr->registerStanzaExtension(new VCardFactory);
	q_ptr->registerStanzaExtension(new PingFactory);
	q_ptr->registerStanzaExtension(new VCardUpdateFactory);
	q_ptr->registerStanzaExtension(new MUCRoomQueryFactory);
	q_ptr->registerStanzaExtension(new MUCRoomUserQueryFactory);
	q_ptr->registerStanzaExtension(new MUCRoomAdminQueryFactory);
	q_ptr->registerStanzaExtension(new MUCRoomOwnerQueryFactory);
	q_ptr->registerStanzaExtension(new EntityTimeFactory);
	q_ptr->registerStanzaExtension(new BookmarkFactory);
	q_ptr->registerStanzaExtension(new PrivateXmlQueryFactory(q_ptr));
	q_ptr->registerStanzaExtension(new PrivacyQueryFactory);
//	client->registerStanzaExtension(new PubSub::EventFactory);
//	client->registerStanzaExtension(new PubSub::PublishFacatory);
	//client->registerStanzaExtension(new PrivateXml::QueryFactory);

	q_ptr->registerStreamFeature(new NonSaslAuth);
	q_ptr->registerStreamFeature(new SASLFeature);
	q_ptr->registerStreamFeature(new TLSFeature);
	q_ptr->registerStreamFeature(new BindFeature);
	q_ptr->registerStreamFeature(new SessionFeature);
	q_ptr->registerStreamFeature(new ZLibCompressionFeature);
	presence.addExtension(new Capabilities(QString(), QLatin1String("http://Jreen.qutim.org/")));
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
	Q_D(Client);
	d->server_port = -1;
	d->init();
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
	return QLatin1Literal("Jreen:") % QString::number(qHash(this), 16) % QLatin1Char(':') % QString::number(d_func()->current_id++);
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
	Q_D(Client);
	if (!d->messageSessionManager)
		d->messageSessionManager = new MessageSessionManager(this);
	return d->messageSessionManager;
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
	qDebug() << Q_FUNC_INFO << d->jid << d->conn << pres.priority();
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

void Client::setConnection(Connection *conn)
{
	Q_D(Client);
	delete d->conn;
	d->conn = conn;
	d->streamProcessor = qobject_cast<StreamProcessor*>(conn);
	d->device->setDevice(conn);
	//	connect(conn, SIGNAL(readyRead()), impl, SLOT(newData()));
	connect(conn, SIGNAL(connected()), this, SLOT(_q_connected()));
	connect(conn, SIGNAL(disconnected()), this, SLOT(_q_disconnected()));
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
	if (d->pingTimer.isActive())
		d->pingTimer.stop();
	if (interval > 0 && isConnected())
		d->pingTimer.start(interval, this);
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
	d->presence.setPriority(priority);
	setPresence();
}

void Client::connectToServer()
{
	Q_D(Client);
	qDebug() << d->server_port;
	if(!d->conn)
		setConnection(new TcpConnection(d->server, d->server_port));

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
		if(force) {
			blockSignals(true);
			d->conn->close();
			d->_q_disconnected();
			blockSignals(false);
			emit disconnected(User);
		}
	}
}

void ClientPrivate::_q_iq_received(const IQ &iq, int context)
{
	Q_UNUSED(context);
	QSharedPointer<Disco::Info> info = iq.findExtension<Disco::Info>();
	if (info) {
		serverFeatures = info->features();
		serverIdentities = info->identities();
		emit q_ptr->serverFeaturesReceived(serverFeatures);
		emit q_ptr->serverIdentitiesReceived(serverIdentities);
		qDebug() << serverFeatures;
	}
}

void Client::timerEvent(QTimerEvent *timerEvent)
{
	Q_D(Client);
	if (timerEvent->timerId() == d->pingTimer.timerId()) {
		IQ iq(IQ::Get, d->jid.bareJID());
		iq.addExtension(new Ping());
		d->send(iq);
	} else {
		return QObject::timerEvent(timerEvent);
	}
}

void Client::handleConnect()
{
	Q_D(Client);
	IQ iq(IQ::Get, d->jid.domain());
	iq.addExtension(new Disco::Info);
	send(iq, this, SLOT(_q_iq_received(Jreen::IQ,int)), 0);
	emit connected();
}

void Client::handleDisconnect()
{
	Q_D(Client);
	DisconnectReason reason = User;

	if(d->conn->socketError() != Connection::UnknownSocketError)
		reason = InternalServerError;
	emit disconnected(reason);
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
//	qDebug() << "Handle message" << message.from();
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

#include "client.moc"
