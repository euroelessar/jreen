/****************************************************************************
 *  client.h
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

#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QSet>
#include "jreen.h"
#include "presence.h"

namespace jreen
{

class ClientPrivate;
class JID;
class Message;
class IQ;
class Connection;
class Subscription;
class StreamFeature;
class Disco;

class XmlStreamHandler
{
public:
	virtual ~XmlStreamHandler();
	
	virtual void handleStreamBegin() = 0;
	virtual void handleStreamEnd() = 0;
	virtual void handleIncomingData(const char *data, qint64 size) = 0;
	virtual void handleOutgoingData(const char *data, qint64 size) = 0;
};

class JREEN_EXPORT Client : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(Client);
	Q_PROPERTY(QSet<QString> serverFeatures READ serverFeatures NOTIFY serverFeaturesReceived)
	ClientPrivate *impl;
	friend class ClientPrivate;
public:
	Client(const JID &jid, const QString &password = QString(), int port = -1);
	Client();
	~Client();
	const JID &jid();
	void setJID(const JID &jid);
	void setPassword(const QString &password);
	void setServer(const QString &server);
	void setResource(const QString &resource);
	void setPort(int port);
	void addXmlStreamHandler(XmlStreamHandler *handler);
	QSet<QString> serverFeatures() const;
	const QString &server() const;
	int port() const;
	const QString getID();
	Presence &presence();
	Disco *disco();
	void send(const Stanza &stanza);
	void send(const IQ &iq, QObject *handler, const char *member, int context);
	void setConnectionImpl(Connection *conn);
	void registerStanzaExtension(StanzaExtension *stanza_extension);
	void registerStanzaExtension(AbstractStanzaExtensionFactory *factory);
	void registerStreamFeature(StreamFeature *stream_feature);
public slots:
	void whitespacePing(int period = 0);
	void setPresence();
	void setPresence(jreen::Presence::Type type, const QString &text = QString(), int priority = -129);
	void connectToServer();
	void disconnectFromServer(bool force = false);
signals:
	void connected();
	void disconnected();
	void authorized();
	void newSubscription(const jreen::Subscription &subscription);
	void newPresence(const jreen::Presence &presence);
	void newIQ(const jreen::IQ &iq);
	void newMessage(const jreen::Message &message);
	void serverFeaturesReceived(const QSet<QString> &features);
protected:
	virtual void handleConnect();
	virtual void handleDisconnect();
	virtual void handleAuthorized();
	virtual void handleSubscription(const Subscription &subscription);
	virtual void handlePresence(const Presence &presence);
	virtual void handleIQ(const IQ &iq);
	virtual void handleMessage(const Message &message);
};

}

#endif // CLIENT_H
