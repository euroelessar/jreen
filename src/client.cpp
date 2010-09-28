/****************************************************************************
 *  client.cpp
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

#include "client_p.h"
#include "stanza_p.h"
#include "tcpconnection.h"
#include "nonsaslauth.h"
#include "delayeddelivery.h"
#include "capabilities.h"
#include "error.h"
#include "dataform.h"

namespace jreen
{

Client::Client(const JID &jid, const QString &password, int port)
	: impl(new ClientPrivate(Presence(Presence::Unavailable,JID()), this))
{
	impl->stream_info = new StreamInfoImpl(impl);
	impl->jid = jid;
	impl->server = jid.domain();
	impl->password = password;
	impl->server_port = port;
	impl->disco = new Disco(this);
	impl->xquery.registerStanzaExtension(new DelayedDelivery, impl->disco);
	impl->xquery.registerStanzaExtension(new Error, impl->disco);
	impl->xquery.registerStanzaExtension(new Capabilities, impl->disco);
	impl->xquery.registerStanzaExtension(new DataForm, impl->disco);
	registerStreamFeature(new NonSaslAuth);
	impl->presence.addExtension(new Capabilities(impl->disco));
}

Client::~Client()
{
	delete impl;
}

const JID &Client::jid()
{
	return impl->jid;
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
	static const QString base_id("jreen");
	return base_id + QString::number(impl->current_id++);
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
//	QDomElement node = stanza;
//	if(stanza.id().isEmpty())
//		node.setAttribute(ConstString::id, getID());
//	impl->send(node);
}

void Client::send(const IQ &iq, QObject *handler, const char *member, int context)
{
	if(!impl->conn || !impl->conn->isOpen())
		return;
	if (iq.id().isEmpty()) {
		const StanzaPrivate *p = StanzaPrivate::get(iq);
		const_cast<StanzaPrivate*>(p)->id = getID();
	}
	QString id = iq.id();
	impl->iq_tracks.insert(id, new IQTrack(handler, member, context));
	impl->send(iq);
}

void Client::setConnectionImpl(Connection *conn)
{
	delete impl->conn;
	impl->conn = conn;
	connect(conn, SIGNAL(readyRead()), impl, SLOT(newData()));
	connect(conn, SIGNAL(connected()), impl, SLOT(connected()));
}

void Client::registerStanzaExtension(StanzaExtension *stanza_extension)
{
	impl->xquery.registerStanzaExtension(stanza_extension, impl->disco);
}

void Client::registerStreamFeature(StreamFeature *stream_feature)
{
	if(!stream_feature)
		return;
	switch(stream_feature->type())
	{
	case StreamFeature::SimpleAuthorization:
		impl->non_sasl_auths.registerStreamFeature(stream_feature);
		break;
	case StreamFeature::SASL:
		impl->sasl_auths.registerStreamFeature(stream_feature);
		break;
	case StreamFeature::CompressionLayer:
		impl->compressions.registerStreamFeature(stream_feature);
		break;
	case StreamFeature::SecurityLayer:
		impl->security_layers.registerStreamFeature(stream_feature);
		break;
	default:
		delete stream_feature;
		return;
	}
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
	send(impl->presence);
}

void Client::connectToServer()
{
	if(!impl->conn || !impl->conn->isOpen())
	{
		if(!impl->conn)
			setConnectionImpl(new TcpConnection(impl->server, impl->server_port));
		impl->conn->open();
	}
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


}
