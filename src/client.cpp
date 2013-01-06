/****************************************************************************
**
** Jreen
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#include "client_p.h"
#include "iq_p.h"
#include "abstractroster.h"
#include "disco_p.h"
#include "stanza_p.h"
#include "tcpconnection.h"
#include "delayeddelivery.h"
#include "dataform.h"
#include "ping.h"
#include "pubsubevent.h"
#include "privatexml_p.h"
#include <QStringBuilder>

// Factories
#include "mucroomfactory_p.h"
#include "entitytimefactory_p.h"
#include "pubsubmanager_p.h"
#include "tunefactory_p.h"
#include "bookmarkfactory_p.h"
#include "metacontactsfactory_p.h"
#include "privacyqueryfactory_p.h"
#include "delayeddeliveryfactory_p.h"
#include "receiptfactory_p.h"
#include "softwareversionfactory_p.h"
#include "moodfactory_p.h"
#include "activityfactory_p.h"
#include "vcardfactory_p.h"
#include "pingfactory_p.h"
#include "vcardupdatefactory_p.h"
#include "iqfactory_p.h"
#include "presencefactory_p.h"
#include "messagefactory_p.h"
#include "chatstatefactory_p.h"
#include "capabilitiesfactory_p.h"
#include "errorfactory_p.h"
#include "registrationqueryfactory_p.h"
#include "bitsofbinaryfactory_p.h"
#include "captchafactory_p.h"
#include "pgpfactory_p.h"
#include "forwardedfactory_p.h"
#include "attentionfactory_p.h"

// Features
#include "nonsaslauth_p.h"
#include "saslfeature_p.h"
#include "tlsfeature_p.h"
#include "bindfeature_p.h"
#include "sessionfeature_p.h"
#include "zlibcompressionfeature_p.h"

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
		if (iq->subtype() == IQ::Result || iq->subtype() == IQ::Error) {
			if (IQReply *reply = iqTracks.take(stanza->id())) {
				emit reply->received(*iq);
				reply->deleteLater();
			}
		} else if (iq->subtype() == IQ::Get || iq->subtype() == IQ::Set) {
			bool ok = iq->from().isDomain()
			        || !iq->from().isValid()
			        || !roster
			        || rooms.contains(iq->from().bare())
			        || iq->from().bare() == jid.bare();
			if (!ok) {
				RosterItem::Ptr item = roster->item(iq->from());
				ok = item
						&& item->subscription() != RosterItem::None
						&& item->subscription() != RosterItem::To;
			}
			if (!ok) {
				IQ error(IQ::Error, iq->from(), iq->id());
				foreach (const Payload::Ptr &se, iq->payloads())
					error.addExtension(se);
				error.addExtension(new Error(Error::Cancel, Error::SubscriptionRequired));
				send(error);
				return;
			}
			q_ptr->handleIQ(*iq);
			if (!iq->accepted()) {
				IQ error(IQ::Error, iq->from(), iq->id());
				error.addExtension(new Error(Error::Cancel, Error::ServiceUnavailable));
				send(error);
			}
		}
	} else if (type == StanzaPrivate::StanzaMessage) {
		q_ptr->handleMessage(*stanza.staticCast<Message>());
	} else if (type == StanzaPrivate::StanzaPresence) {
		if (MUCRoomPrivate *room = rooms.value(stanza->from().bare())) {
			room->handlePresence(*stanza.staticCast<Presence>());
			emit q_ptr->mucPresenceReceived(*stanza.staticCast<Presence>());
		} else {
			q_ptr->handlePresence(*stanza.staticCast<Presence>());
		}
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

	q_ptr->registerPayload(new ErrorFactory);
	q_ptr->registerPayload(capsFactory);
	q_ptr->registerPayload(new DataFormFactory);
	q_ptr->registerPayload(new DiscoInfoFactory);
	q_ptr->registerPayload(new DiscoItemsFactory);
	q_ptr->registerPayload(new ChatStateFactory);
	q_ptr->registerPayload(new DelayedDeliveryFactory);
	q_ptr->registerPayload(new ReceiptFactory);
	q_ptr->registerPayload(new SoftwareVersionFactory);
	q_ptr->registerPayload(new MoodFactory);
	q_ptr->registerPayload(new TuneFactory);
	q_ptr->registerPayload(new ActivityFactory);
	q_ptr->registerPayload(new VCardFactory);
	q_ptr->registerPayload(new PingFactory);
	q_ptr->registerPayload(new VCardUpdateFactory);
	q_ptr->registerPayload(new MUCRoomQueryFactory);
	q_ptr->registerPayload(new MUCRoomUserQueryFactory);
	q_ptr->registerPayload(new MUCRoomAdminQueryFactory);
	q_ptr->registerPayload(new MUCRoomOwnerQueryFactory);
	q_ptr->registerPayload(new EntityTimeFactory);
	q_ptr->registerPayload(new BookmarkFactory);
	q_ptr->registerPayload(new MetaContactsFactory);
	q_ptr->registerPayload(new PrivateXmlQueryFactory(q_ptr));
	q_ptr->registerPayload(new PrivacyQueryFactory);
	q_ptr->registerPayload(new RegistrationQueryFactory);
	q_ptr->registerPayload(new BitsOfBinaryFactory);
	q_ptr->registerPayload(new CaptchaFactory);
	q_ptr->registerPayload(new PGPSignedFactory);
	q_ptr->registerPayload(new PGPEncryptedFactory);
	q_ptr->registerPayload(new ForwardedFactory(q_ptr));
	q_ptr->registerPayload(new AttentionFactory);

	q_ptr->registerStreamFeature(new NonSaslAuth);
	q_ptr->registerStreamFeature(new SASLFeature);
	q_ptr->registerStreamFeature(new TLSFeature);
	q_ptr->registerStreamFeature(new BindFeature);
	q_ptr->registerStreamFeature(new SessionFeature);
	q_ptr->registerStreamFeature(new ZLibCompressionFeature);
	presence.addExtension(new Capabilities(QString(), QLatin1String("http://qutim.org/jreen")));
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

void Client::setProxy(const QNetworkProxy &proxy)
{
	Q_D(Client);
	d->proxy = proxy;
}

void Client::setFeatureConfig(Client::Feature feature, Client::FeatureConfig config)
{
	Q_D(Client);
	if (feature < 0 || feature >= d->configs.size())
		return;
	d->configs[feature] = config;
}

Client::FeatureConfig Client::featureConfig(Client::Feature feature) const
{
	Q_D(const Client);
	return d->configs.value(feature, Auto);
}

bool Client::isFeatureActivated(Client::Feature feature) const
{
	Q_D(const Client);
	return (d->usedFeatures & (1 << feature));
}

QNetworkProxy Client::proxy() const
{
	return d_func()->proxy;
}

void Client::setProxyFactory(QNetworkProxyFactory *factory)
{
	d_func()->proxyFactory.reset(factory);
}

QNetworkProxyFactory *Client::proxyFactory() const
{
	return d_func()->proxyFactory.data();
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

JingleManager *Client::jingleManager()
{
	Q_D(Client);
	if (!d->jingleManager)
		d->jingleManager.reset(new JingleManager(this));
	return d->jingleManager.data();
}

void Client::send(const Stanza &stanza)
{
	Q_D(Client);
	if(!d->conn || !d->conn->isOpen() || !d->isConnected)
		return;
	d->send(stanza);
}

void Client::send(const Presence &pres)
{
	Q_D(Client);
	Logger::debug() << Q_FUNC_INFO << d->jid << d->conn << pres.priority();
	Logger::debug() << d->conn->isOpen();
	if(!d->conn || !d->conn->isOpen() || !d->isConnected)
		return;
	if (StanzaPrivate::get(pres) == StanzaPrivate::get(d->presence)) {
		d->send(pres);
		return;
	}
	Presence p = pres;
	foreach (const Payload::Ptr &se, d->presence.payloads())
		p.addExtension(se);
	d->send(p);
}

IQReply *Client::send(const IQ &iq)
{
	Q_D(Client);
	if(!d->conn || !d->conn->isOpen())
		return 0;
	if (!d->isConnected && !IQPrivate::isConnection(iq))
		return 0;
	if (iq.id().isEmpty()) {
		const StanzaPrivate *p = StanzaPrivate::get(iq);
		const_cast<StanzaPrivate*>(p)->id = getID();
	}

	Logger::debug() << "send iq to" << iq.to() << "from" << iq.from();
	d->send(iq);
	if (iq.subtype() == IQ::Set || iq.subtype() == IQ::Get) {
		IQReply *reply = d->createIQReply();
		d->iqTracks.insert(iq.id(), reply);
		return reply;
	}
	return 0;
}

void Client::send(const IQ &iq, QObject *handler, const char *member, int context)
{
	Q_D(Client);
	if(!d->conn || !d->conn->isOpen())
		return;
	if (!d->isConnected && !IQPrivate::isConnection(iq))
		return;
	if (iq.id().isEmpty()) {
		const StanzaPrivate *p = StanzaPrivate::get(iq);
		const_cast<StanzaPrivate*>(p)->id = getID();
	}

	Logger::debug() << "send iq to" << iq.to() << "from" << iq.from();
	d->send(iq);
	if (iq.subtype() == IQ::Set || iq.subtype() == IQ::Get) {
		IQReply *reply = new IQTrack(handler, member, context, this);
		d->iqTracks.insert(iq.id(), reply);
	}
}

void Client::setConnection(Connection *conn)
{
	Q_D(Client);
	delete d->conn;
	d->conn = conn;
	d->streamProcessor = qobject_cast<StreamProcessor*>(conn);
	d->bufferedDevice->setDevice(conn);
	//	connect(conn, SIGNAL(readyRead()), impl, SLOT(newData()));
	connect(conn, SIGNAL(connected()), this, SLOT(_q_connected()));
	connect(conn, SIGNAL(stateChanged(Jreen::Connection::SocketState)),
	        this, SLOT(_q_stateChanged(Jreen::Connection::SocketState)));
}

Connection *Client::connection() const
{
	return d_func()->conn;
}

void Client::registerPayload(AbstractPayloadFactory *factory)
{
	Q_D(Client);
//	delete d->factories.value(factory->payloadType(), 0);
	d->factories.insert(factory->payloadType(), factory);
	foreach (const QString &feature, factory->features()) {
		DiscoPrivate::get(d->disco)->features << feature;
		d->factoriesByUri.insert(feature, factory);
	}
}

inline bool featureLessThan(StreamFeature *a, StreamFeature *b)
{
	return a->type() == b->type() ? a->priority() > b->priority() : a->type() < b->type();
}

void Client::registerStreamFeature(StreamFeature *streamFeature)
{
	Q_D(Client);
	if(!streamFeature)
		return;
	d->features.insert(qLowerBound(d->features.begin(), d->features.end(),
	                               streamFeature, featureLessThan), streamFeature);
	streamFeature->setStreamInfo(d->stream_info);
}

void Client::removeStreamFeature(StreamFeature *streamFeature)
{
	Q_D(Client);
	d->features.removeAll(streamFeature);
	streamFeature->setStreamInfo(0);
}

void Client::setPingInterval(int interval)
{
	Q_D(Client);
	d->pingInterval = interval;
	if (interval <= 0)
		d->pingTimer.stop();
	else if (isConnected())
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
	if(!d->conn)
		setConnection(new TcpConnection(d->server, d->server_port));

	if(!d->conn->isOpen()) {
		if (DirectConnection *connection = qobject_cast<DirectConnection*>(d->conn)) {
			QNetworkProxy proxy;
			if (d->proxyFactory) {
				QUrl url = QUrl::fromUserInput(d->jid);
				url.setScheme(QLatin1String("xmpp"));
				QNetworkProxyQuery query(url);
				proxy = d->proxyFactory->queryProxy(query).value(0);
			} else {
				proxy = d->proxy;
			}
			connection->setProxy(proxy);
		}
		
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
	QSharedPointer<Disco::Info> info = iq.payload<Disco::Info>();
	if (info) {
		serverFeatures = info->features();
		serverIdentities = info->identities();
		emit q_ptr->serverFeaturesReceived(serverFeatures);
		emit q_ptr->serverIdentitiesReceived(serverIdentities);
		Logger::debug() << serverFeatures;
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
	if (d->pingInterval > 0)
		d->pingTimer.start(d->pingInterval, this);
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

void Client::handlePresence(const Presence &presence)
{
	emit presenceReceived(presence);
}

void Client::handleIQ(const IQ &iq)
{
	//handle XMPP::ping
	if(iq.containsPayload<Ping>()) {
		iq.accept();
		IQ pong(IQ::Result,iq.from(),iq.id());
		pong.setFrom(d_func()->jid);
		send(pong); //FIXME remove warning
	}
	if (iq.containsPayload<EntityTime>()) {
		iq.accept();
		IQ result(IQ::Result,iq.from(),iq.id());
		result.addExtension(new EntityTime(QDateTime::currentDateTime()));
		send(result);
	}
	emit iqReceived(iq);
}

void Client::handleMessage(const Message &message)
{
	Logger::debug() << "Handle message" << message.from();
	emit messageReceived(message);
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

#include "moc_client.cpp"
