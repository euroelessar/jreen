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

#include "directconnection_p.h"
#include <QSslSocket>
#include <QSslConfiguration>
#include <QSsl>

#ifdef Q_OS_LINUX
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <netinet/tcp.h>
#endif

namespace Jreen
{

DirectConnectionPrivate::DirectConnectionPrivate(const QString &hn, int p, DirectConnection *par)
		: host_name(hn), port(p), dns_lookup_id(-1), parent(par)
{
	do_lookup = p < 0 || !QUrl(host_name).isValid();
	socket_state = QAbstractSocket::UnconnectedState;
	socket_error = QAbstractSocket::UnknownSocketError;
}

void DirectConnectionPrivate::connectSocket()
{
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

void DirectConnectionPrivate::doLookup()
{
	Logger::debug() << "doLookup";
	emit stateChanged(QAbstractSocket::HostLookupState);

	if (SJDns::instance().isValid())
		SJDns::instance().doLookup(host_name, this, SLOT(lookupResultsReady()));
	else
		emit stateChanged(QAbstractSocket::UnconnectedState);
}

void DirectConnectionPrivate::lookupResultsReady()
{
	const QJDns::Response *response = SJDns::instance().servers(host_name);
	dns_records.clear();
	if (!response || !response->answerRecords.size()) {
		Record record;
		record.host = host_name;
		dns_records << record;
	} else {
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
	Logger::debug() << "use:" << record.host << record.port;
	socket->connectToHost(record.host, record.port);
}

void DirectConnectionPrivate::stateChanged(QAbstractSocket::SocketState ss)
{
	Logger::debug() << ss;
	if(socket_state == ss)
		return;

	switch(ss) {
	case QAbstractSocket::ConnectedState: {
	// It's known that KeepAlive breaks connections on Maemo 5 and MeeGo Harmattan
#if defined(Q_OS_LINUX) && !defined(Q_WS_MAEMO_5) && !defined(MEEGO_EDITION_HARMATTAN)
		if (qobject_cast<QTcpSocket*>(socket)) {
			int fd = socket->socketDescriptor();
			Q_ASSERT(fd != -1);
			Logger::debug() << "Trying to set KeepAlive attributes to socket descriptor" << fd;
			if (fd != -1) {
				socket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
				int enableKeepAlive = 1;
				Logger::debug() << setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &enableKeepAlive, sizeof(enableKeepAlive));
			
				int maxIdle = 15; // seconds
				Logger::debug() << setsockopt(fd, IPPROTO_TCP, TCP_KEEPIDLE, &maxIdle, sizeof(maxIdle));
			
				int count = 3;  // send up to 3 keepalive packets out, then disconnect if no response
				Logger::debug() << setsockopt(fd, SOL_TCP, TCP_KEEPCNT, &count, sizeof(count));
			
				int interval = 2;   // send a keepalive packet out every 2 seconds (after the idle period)
				Logger::debug() << setsockopt(fd, SOL_TCP, TCP_KEEPINTVL, &interval, sizeof(interval));
			}
		}
#endif
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

void DirectConnectionPrivate::error(QAbstractSocket::SocketError se)
{
	socket_error = se;
	emit parent->error(static_cast<Connection::SocketError>(se));
}

DirectConnection::DirectConnection(QAbstractSocket *socket, const QString &host_name, qint16 port)
	: d_ptr(new DirectConnectionPrivate(host_name, port, this))
{
	Q_ASSERT(socket);
	d_ptr->socket = socket;
	d_ptr->connectSocket();
}

DirectConnection::DirectConnection(QAbstractSocket *socket, const QHostAddress &address, qint16 port)
	: d_ptr(new DirectConnectionPrivate(address.toString(), port, this))
{
	Q_ASSERT(socket);
	d_ptr->socket = socket;
	d_ptr->connectSocket();
}

DirectConnection::~DirectConnection()
{
	delete d_ptr->socket;
}

bool DirectConnection::open()
{
	Q_D(DirectConnection);
	if(d->socket_state != QAbstractSocket::UnconnectedState) {
		if(d->socket_state == QAbstractSocket::ListeningState) {
			d->socket_state = QAbstractSocket::ConnectedState;
			QIODevice::open(ReadWrite);
			emit stateChanged(static_cast<SocketState>(d->socket_state));
		}
		return true;
	}
	if(d->do_lookup) {
		d->doLookup();
	} else {
		Logger::debug() << "connectToHost" << d->host_name << d->port;
		Logger::debug() << "proxy" << d->socket->proxy().type() << d->socket->proxy().hostName() << d->socket->proxy().port();
		if (QSslSocket *socket = qobject_cast<QSslSocket*>(d->socket)) {
			socket->setPeerVerifyMode(QSslSocket::VerifyNone);
			QSslConfiguration conf = socket->sslConfiguration();
			conf.setProtocol(QSsl::TlsV1);
			socket->setSslConfiguration(conf);
			socket->connectToHostEncrypted(d->host_name, d->port);
		} else {
			d->socket->connectToHost(d->host_name, d->port);
		}
	}
	return true;
}

void DirectConnection::close()
{
	Q_D(DirectConnection);
	d->socket->flush();
	d->socket->disconnectFromHost();
	QIODevice::close();
}

void DirectConnection::setHost(const QHostAddress &host)
{
	d_func()->host_name = host.toString();
}

void DirectConnection::setHost(const QString &host)
{
	d_func()->host_name = host;
}

void DirectConnection::setPort(int port)
{
	d_func()->port = port;
}

void DirectConnection::setProxy(const QNetworkProxy &proxy)
{
	d_func()->socket->setProxy(proxy);
}

qint64 DirectConnection::bytesAvailable() const
{
	return d_func()->socket->bytesAvailable() + QIODevice::bytesAvailable();
}

Connection::SocketState DirectConnection::socketState() const
{
	Q_D(const DirectConnection);
	return static_cast<SocketState>(d->socket_state);
}

Connection::SocketError DirectConnection::socketError() const
{
	Q_D(const DirectConnection);
	return static_cast<SocketError>(d->socket_error);
}

qint64 DirectConnection::readData(char *data, qint64 maxlen)
{
	Q_D(DirectConnection);
	return d->socket->read(data, maxlen);
}

qint64 DirectConnection::writeData(const char *data, qint64 len)
{
	Q_D(DirectConnection);
	return d->socket->write(data, len);
}

}
