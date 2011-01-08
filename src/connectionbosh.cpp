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

#include "connectionbosh.h"
#include "util.h"
#include "jid.h"
#include <QCryptographicHash>

namespace jreen
{
class ConnectionBOSHPrivate
{
public:
	qint64 rid;
	QString sessionId;
	QByteArray key;
	int keyNum;
	int keyIndex;
	JID jid;
	XmlStreamParser *streamParser;
	
	QByteArray generateKey()
	{
		keyIndex = 0;
		key = Util::randomHash();
		return key;
	}
	QByteArray nextKey()
	{
		key = QCryptographicHash::hash(key, QCryptographicHash::Sha1).toHex();
		return key;
	}
	void send(const QByteArray &data);
	void sendHeader(bool first);
};

void ConnectionBOSHPrivate::send(const QByteArray &data)
{
	qDebug() << data;
}

void ConnectionBOSHPrivate::sendHeader(bool first)
{
	QByteArray data;
	QXmlStreamWriter writer(&data);
	writer.writeAttribute(QLatin1String("rid"), QString::number(rid++));
	if (first) {
		writer.writeAttribute(QLatin1String("from"), jid.bare());
		writer.writeAttribute(QLatin1String("route"),
							  QLatin1String("xmpp:") + jid.domain() + QLatin1String(":5222"));
		writer.writeAttribute(QLatin1String("secure"), QLatin1String("true"));
		writer.writeAttribute(QLatin1String("hold"), QLatin1String("2"));
		writer.writeAttribute(QLatin1String("ver"), QLatin1String("1.6"));
		writer.writeAttribute(QLatin1String("xmpp:version"), QLatin1String("1.0"));
		writer.writeAttribute(QLatin1String("xml:lang"), QLatin1String("en"));
	} else {
		writer.writeAttribute(QLatin1String("key"), QLatin1String(nextKey()));
		writer.writeAttribute(QLatin1String("sid"), sessionId);
	}
	writer.writeAttribute(QLatin1String("newkey"), QLatin1String(generateKey()));
	writer.writeAttribute(QLatin1String("to"), jid.domain());
	writer.writeDefaultNamespace(QLatin1String("http://jabber.org/protocol/httpbind"));
	writer.writeAttribute(QLatin1String("xmlns:xmpp"), QLatin1String("urn:xmpp:xbosh"));
	writer.writeCharacters(QLatin1String(""));
	send(data);
}

ConnectionBOSH::ConnectionBOSH() : d_ptr(new ConnectionBOSHPrivate)
{
	Q_D(ConnectionBOSH);
	d->rid = 0;
	d->streamParser = 0;
}

ConnectionBOSH::~ConnectionBOSH()
{
}

bool ConnectionBOSH::open()
{
	Q_D(ConnectionBOSH);
	d->rid = (uint64(qrand()) << 20) ^ uint64(qrand());
	d->generateKey();
	d->keyNum = qAbs(qrand()) % 100 + 50;
	d->sendHeader(true);
	return true;
}

void ConnectionBOSH::close()
{
}

qint64 ConnectionBOSH::bytesAvailable() const
{
	return 0;
}

ConnectionBOSH::SocketState ConnectionBOSH::socketState() const
{
	return ConnectedState;
}

ConnectionBOSH::SocketError ConnectionBOSH::socketError() const
{
	return UnknownSocketError;
}

QString ConnectionBOSH::sessionID() const
{
	return d_func()->sessionID;
}

void ConnectionBOSH::setJID(const JID &jid)
{
	d_func()->jid = jid;
}

void ConnectionBOSH::setStreamParser(XmlStreamParser *parser)
{
	d_func()->streamParser = parser;
}

void ConnectionBOSH::restartStream()
{
	Q_D(ConnectionBOSH);
	
//	rid='1573741824'
//	sid='SomeSID'
//	to='example.com'
//	xml:lang='en'
//	xmpp:restart='true'
//	xmlns='http://jabber.org/protocol/httpbind'
//	xmlns:xmpp='urn:xmpp:xbosh'
}

qint64 readData(char *data, qint64 maxlen)
{
	Q_UNUSED(data);
	Q_UNUSED(maxlen);
	return 0;
}

qint64 writeData(const char *payloaddata, qint64 payloadlen)
{
	Q_D(ConnectionBOSH);
	QByteArray data;
	QXmlStreamWriter writer(&data);
	writer.writeStartElement(QLatin1String("body"));
	writer.writeAttribute(QLatin1String("rid"), QString::number(d->rid++));
	writer.writeAttribute(QLatin1String("sid"), d->sessionId);
	writer.writeAttribute(QLatin1String("key"), QLatin1String(d->nextKey()));
	if (d->keyIndex >= d->keyNum)
		writer.writeAttribute(QLatin1String("newkey"), QLatin1String(d->generateKey()));
	writer.writeDefaultNamespace(QLatin1String("http://jabber.org/protocol/httpbind"));
	writer.writeCharacters(QLatin1String(""));
	data.append(payloaddata, payloadlen);
	writer.writeEndElement();
	send(data);
}
}
