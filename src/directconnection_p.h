/****************************************************************************
 *  directconnection_p.h
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

#ifndef DIRECTCONNECTION_P_H
#define DIRECTCONNECTION_P_H

#include <QAbstractSocket>
#include <QHostAddress>
#include "jreen.h"
#include "directconnection.h"
#include "sjdns_p.h"

J_BEGIN_NAMESPACE

class DirectConnectionPrivate : public QObject
{
	Q_OBJECT
public:
	struct Record
	{
		Record() : port(5222), weight(0), priority(0) {}
		QString host;
		int port;
		int weight;
		int priority;
	};
	DirectConnectionPrivate( const QString &hn, int p, DirectConnection *par )
			: host_name(hn), port(p), dns_lookup_id(-1), parent(par)
	{
		do_lookup = p < 0 || QHostAddress(host_name).isNull();
		socket_state = QAbstractSocket::UnconnectedState;
		socket_error = QAbstractSocket::UnknownSocketError;
	}
	void connectSocket()
	{
		connect( socket, SIGNAL(connected()), parent, SIGNAL(connected()) );
		connect( socket, SIGNAL(disconnected()), parent, SIGNAL(disconnected()) );
		connect( socket, SIGNAL(readyRead()), parent, SIGNAL(readyRead()) );
		connect( socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(stateChanged(QAbstractSocket::SocketState)) );
		connect( socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error(QAbstractSocket::SocketError)) );
	}
	void doLookup()
	{
		stateChanged( QAbstractSocket::HostLookupState );
		SJDns::instance().doLookup( host_name, this, SLOT(lookupResultsReady()) );
	}
	QAbstractSocket *socket;
	QString host_name;
	int port;
	bool do_lookup;
	QAbstractSocket::SocketState socket_state;
	QAbstractSocket::SocketError socket_error;
	int dns_lookup_id;
	QList<Record> dns_records;
	DirectConnection *parent;
public slots:
	void lookupResultsReady()
	{
		const QJDns::Response *response = SJDns::instance().servers( host_name );
		dns_records.clear();
		if( !response || !response->answerRecords.size() )
		{
			Record record;
			record.host = host_name;
			dns_records << record;
		}
		else
		{
			foreach( const QJDns::Record &qrecord, response->answerRecords )
			{
				Record record;
				record.host = QString::fromUtf8( qrecord.name );
				record.port = qrecord.port;
				record.weight = qrecord.weight;
				record.priority = qrecord.priority;
				dns_records << record;
			}
		}
		Record &record = dns_records[0];
		qDebug() << "use:" << record.host << record.port;
		socket->connectToHost( record.host, record.port );
	}
	void stateChanged( QAbstractSocket::SocketState ss )
	{
		qDebug() << ss;
		if( socket_state == ss )
			return;
		if( ss == QAbstractSocket::ConnectedState )
		{
			socket_state = QAbstractSocket::ListeningState;
			parent->open();
			return;
		}
		socket_state = ss;
		emit parent->stateChanged( static_cast<Connection::SocketState>( ss ) );
	}
	void error( QAbstractSocket::SocketError se )
	{
		socket_error = se;
		emit parent->error( static_cast<Connection::SocketError>( se ) );
	}
};

J_END_NAMESPACE

#endif // DIRECTCONNECTION_P_H
