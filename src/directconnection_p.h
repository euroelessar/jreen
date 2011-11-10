/****************************************************************************
**
** Jreen
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef DIRECTCONNECTION_P_H
#define DIRECTCONNECTION_P_H

#include <QSslSocket>
#include <QHostAddress>
#include <QNetworkProxy>
#include "jreen.h"
#include "directconnection.h"
#include "sjdns_p.h"
#include <QDebug>
#include <QUrl>

namespace Jreen
{

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
	DirectConnectionPrivate(const QString &hn, int p, DirectConnection *par)
			: host_name(hn), port(p), dns_lookup_id(-1), parent(par)
	{
		do_lookup = p < 0 || !QUrl(host_name).isValid();
		socket_state = QAbstractSocket::UnconnectedState;
		socket_error = QAbstractSocket::UnknownSocketError;
	}
	void connectSocket()
	{
//		QNetworkProxy proxy;
//		proxy.setType(QNetworkProxy::HttpProxy);
//		proxy.setHostName("proxy.istu.ru");
//		proxy.setPort(8080);
//		socket->setProxy(proxy);
		if (qobject_cast<QSslSocket*>(socket)) {
			connect(socket, SIGNAL(encrypted()), parent, SIGNAL(connected()));
		} else {
			connect(socket, SIGNAL(connected()), parent, SIGNAL(connected()));
		}
		connect(socket, SIGNAL(disconnected()), parent, SIGNAL(disconnected()));
		connect(socket, SIGNAL(readyRead()), parent, SIGNAL(readyRead()));
		connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
		        this, SLOT(stateChanged(QAbstractSocket::SocketState)));
		connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
		        this, SLOT(error(QAbstractSocket::SocketError)));
		connect(socket, SIGNAL(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)),
		        parent, SIGNAL(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)));
	}
	void doLookup()
	{
		qDebug() << "doLookup";
		stateChanged(QAbstractSocket::HostLookupState);

		SJDns::instance().doLookup(host_name, this, SLOT(lookupResultsReady()));
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
		const QJDns::Response *response = SJDns::instance().servers(host_name);
		dns_records.clear();
		if(!response || !response->answerRecords.size()) {
			Record record;
			record.host = host_name;
			dns_records << record;
		}
		else {
			foreach(const QJDns::Record &qrecord, response->answerRecords)	{
				Record record;
				record.host = QUrl::fromAce(qrecord.name);
				// may be it's a reason of connection problems of some users
				if (record.host.endsWith(QLatin1Char('.')))
					record.host.chop(1);
				record.port = qrecord.port;
				record.weight = qrecord.weight;
				record.priority = qrecord.priority;
				dns_records << record;
			}
		}
		Record &record = dns_records[0];
		qDebug() << "use:" << record.host << record.port;
		socket->connectToHost(record.host, record.port);
	}
	void stateChanged(QAbstractSocket::SocketState ss)
	{
		qDebug() << ss;
		if(socket_state == ss)
			return;

		switch(ss) {
		case QAbstractSocket::ConnectedState: {
			socket_state = QAbstractSocket::ListeningState;
			parent->open();
			return;
		}
		case QAbstractSocket::ClosingState:
			parent->close();
			break;
		default:
			break;
		}

		socket_state = ss;
		emit parent->stateChanged(static_cast<Connection::SocketState>(ss));
	}
	void error(QAbstractSocket::SocketError se)
	{
		socket_error = se;
		emit parent->error(static_cast<Connection::SocketError>(se));
	}
};

}

#endif // DIRECTCONNECTION_P_H
