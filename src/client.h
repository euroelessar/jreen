/****************************************************************************
 *  client.h
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

class JREEN_EXPORT Client : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(Client);
	ClientPrivate *impl;
	friend class ClientPrivate;
public:
	Client(const JID &jid, const QString &password = QString(), int port = -1);
	~Client();
	const JID &jid();
	void setPassword(const QString &password);
	void setServer(const QString &server);
	void setResource(const QString &resource);
	void setPort(int port);
	const QString &server() const;
	int port() const;
	const QString getID();
	Presence &presence();
	Disco *disco();
	void send(const Stanza &stanza);
	void send(const IQ &iq, QObject *handler, const char *member, int context);
	void setConnectionImpl(Connection *conn);
	void registerStanzaExtension(StanzaExtension *stanza_extension);
	void registerStreamFeature(StreamFeature *stream_feature);
public slots:
	void whitespacePing(int period = 0);
	void setPresence();
	void setPresence(Presence::Type type, const QString &text = QString(), int priority = -129);
	void connectToServer();
	void disconnectFromServer(bool force = false);
signals:
	void onConnect();
	void onDisconnect();
	void authorized();
	void newSubscription(const Subscription &subscription);
	void newPresence(const Presence &presence);
	void newIQ(const IQ &iq);
	void newMessage(const Message &message);
protected:
	virtual void handleConnect() { emit onConnect(); }
	virtual void handleDisconnect() { emit onDisconnect(); }
	virtual void handleAuthorized() { emit authorized(); }
	virtual void handleSubscription(const Subscription &subscription) { emit newSubscription(subscription); }
	virtual void handlePresence(const Presence &presence) { emit newPresence(presence); }
	virtual void handleIQ(const IQ &iq) { emit newIQ(iq); }
	virtual void handleMessage(const Message &message) { emit newMessage(message); }
};

}

#endif // CLIENT_H
