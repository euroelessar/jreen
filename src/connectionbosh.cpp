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
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QDebug>
#include <QBuffer>

namespace jreen
{
class ConnectionBOSHPrivate
{
public:
	ConnectionBOSHPrivate() : resultBuffer(&resultXml), writer(&resultBuffer) {}
	quint64 rid;
	QString sessionId;
	QByteArray key;
	int keyNum;
	int keyIndex;
	JID jid;
	XmlStreamParser *streamParser;
	QNetworkAccessManager manager;
	QUrl host;
	QNetworkReply *emptyRequest;
	QNetworkReply *dataRequest;
	QByteArray resultXml;
	QBuffer resultBuffer;
	QXmlStreamWriter writer;
	QXmlStreamReader reader;
	
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
	void send();
	void sendHeader(bool first);
};

void ConnectionBOSHPrivate::send()
{
	QByteArray data = resultXml;
	resultBuffer.seek(0);
	resultXml.clear();
	qDebug() << Q_FUNC_INFO << data;
	QNetworkRequest request(host);
//	request.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);
	request.setHeader(QNetworkRequest::ContentTypeHeader, QByteArray("text/xml; charset=utf-8"));
//	request.setRawHeader("Accept-Encoding", "gzip, deflate");
	manager.post(request, data);
}

void ConnectionBOSHPrivate::sendHeader(bool first)
{
	writer.writeEmptyElement(QLatin1String("body"));
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
	send();
}

ConnectionBOSH::ConnectionBOSH(const QString &host, int port) : d_ptr(new ConnectionBOSHPrivate)
{
	Q_D(ConnectionBOSH);
	d->rid = 0;
	d->streamParser = 0;
	d->host.setScheme(QLatin1String("http"));
	d->host.setHost(host);
	d->host.setPort(port);
	d->host.setPath(QLatin1String("http-bind"));
	d->resultBuffer.open(QIODevice::ReadWrite);
	d->writer.writeStartDocument();
	d->writer.writeStartElement(QLatin1String("stream"));
	d->writer.writeCharacters(QLatin1String(""));
	d->reader.addData(d->resultXml);
	while (d->reader.readNext() > QXmlStreamReader::Invalid);
	d->resultBuffer.seek(0);
	d->resultXml.clear();
	connect(&d->manager, SIGNAL(finished(QNetworkReply*)), SLOT(onRequestFinished(QNetworkReply*)));
}

ConnectionBOSH::~ConnectionBOSH()
{
}

bool ConnectionBOSH::open()
{
	Q_D(ConnectionBOSH);
	d->rid = (quint64(qrand()) << 20) ^ quint64(qrand());
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
	return d_func()->sessionId;
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
	d->sendHeader(false);
}

qint64 ConnectionBOSH::readData(char *data, qint64 maxlen)
{
	Q_UNUSED(data);
	Q_UNUSED(maxlen);
	return 0;
}

qint64 ConnectionBOSH::writeData(const char *payloaddata, qint64 payloadlen)
{
	Q_D(ConnectionBOSH);
	d->writer.writeStartElement(QLatin1String("body"));
	d->writer.writeAttribute(QLatin1String("rid"), QString::number(d->rid++));
	d->writer.writeAttribute(QLatin1String("sid"), d->sessionId);
	d->writer.writeAttribute(QLatin1String("key"), QLatin1String(d->nextKey()));
	if (d->keyIndex >= d->keyNum)
		d->writer.writeAttribute(QLatin1String("newkey"), QLatin1String(d->generateKey()));
	d->writer.writeDefaultNamespace(QLatin1String("http://jabber.org/protocol/httpbind"));
	d->writer.writeCharacters(QLatin1String(""));
	d->resultXml.append(payloaddata, payloadlen);
	d->writer.writeEndElement();
	d->send();
	return payloadlen;
}

void ConnectionBOSH::onRequestFinished(QNetworkReply *reply)
{
	Q_D(ConnectionBOSH);
	qDebug() << reply->rawHeaderList();
	qDebug() << Q_FUNC_INFO << reply->error() << reply->errorString();
	if (reply->error() != QNetworkReply::NoError) {
		// TODO: Implement
		return;
	}
	QByteArray data = reply->readAll();
	qDebug() << Q_FUNC_INFO << data;
	d->reader.addData(data);
	int depth = 0;
	while (d->reader.readNext() > QXmlStreamReader::Invalid) {
		switch(d->reader.tokenType()) {
		case QXmlStreamReader::StartElement:
			depth++;
			if (depth > 1) {
				d->streamParser->handleStartElement(d->reader.name(), d->reader.namespaceUri(),
													d->reader.attributes());
			} else {
				
			}
			break;
		case QXmlStreamReader::EndElement:
			if (depth > 1)
				d->streamParser->handleEndElement(d->reader.name(), d->reader.namespaceUri());
			depth--;
			break;
		case QXmlStreamReader::Characters:
			if (depth > 1)
				d->streamParser->handleCharacterData(d->reader.text());
			break;
		default:
			break;
		}
	}
}
}
