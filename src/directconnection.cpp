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

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#  include "sjdns_p.h"
#else
#  include <QDnsLookup>
#endif

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
	connect(socket, SIGNAL(connected()), parent, SIGNAL(connected()));
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
	jreenDebug() << "doLookup";
	emit stateChanged(QAbstractSocket::HostLookupState);

	QDnsLookup *dns = new QDnsLookup(this);
	connect(dns, SIGNAL(finished()), this, SLOT(lookupResultsReady()));

	dns->setType(QDnsLookup::SRV);
	dns->setName(QString::fromLatin1("_xmpp-client._tcp." + QUrl::toAce(host_name)));
	dns->lookup();
}

void DirectConnectionPrivate::lookupResultsReady()
{
	QDnsLookup *dns = qobject_cast<QDnsLookup*>(sender());
	dns->deleteLater();
	Q_ASSERT(dns);

	QList<QDnsServiceRecord> results = dns->serviceRecords();
	dns_records.clear();

	if (results.isEmpty()) {
		Record record;
		record.host = host_name;
		dns_records << record;
	} else {
		foreach (const QDnsServiceRecord &result, results) {
			Record record;
			record.host = result.target();
			// may be it's a reason of connection problems of some users
			if (record.host.endsWith(QLatin1Char('.')))
				record.host.chop(1);
			record.port = result.port();
			record.weight = result.weight();
			record.priority = result.priority();
			dns_records << record;
		}
	}

	Record &record = dns_records[0];
	jreenDebug() << "use:" << record.host << record.port;
	socket->connectToHost(record.host, record.port);
}

void DirectConnectionPrivate::stateChanged(QAbstractSocket::SocketState ss)
{
	jreenDebug() << Q_FUNC_INFO << socket_state << ss;
	if(socket_state == ss)
		return;

	switch(ss) {
	case QAbstractSocket::ConnectedState: {
	// It's known that KeepAlive breaks connections on Maemo 5 and MeeGo Harmattan
#if defined(Q_OS_LINUX) && !defined(Q_WS_MAEMO_5) && !defined(MEEGO_EDITION_HARMATTAN)
		if (qobject_cast<QTcpSocket*>(socket)) {
			int fd = socket->socketDescriptor();
			if (fd != -1) {
				jreenDebug() << "Trying to set KeepAlive attributes to socket descriptor" << fd;
				if (fd != -1) {
					socket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
					int enableKeepAlive = 1;
					jreenDebug() << setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &enableKeepAlive, sizeof(enableKeepAlive));

					int maxIdle = 15; // seconds
					jreenDebug() << setsockopt(fd, IPPROTO_TCP, TCP_KEEPIDLE, &maxIdle, sizeof(maxIdle));

					int count = 3;  // send up to 3 keepalive packets out, then disconnect if no response
					jreenDebug() << setsockopt(fd, SOL_TCP, TCP_KEEPCNT, &count, sizeof(count));

					int interval = 2;   // send a keepalive packet out every 2 seconds (after the idle period)
					jreenDebug() << setsockopt(fd, SOL_TCP, TCP_KEEPINTVL, &interval, sizeof(interval));
				}
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

	socket_state = socket->state();
	emit parent->stateChanged(static_cast<Connection::SocketState>(socket_state));
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
	jreenDebug() << Q_FUNC_INFO << d->socket_state << d->socket->state();
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
		jreenDebug() << "connectToHost" << d->host_name << d->port;
		jreenDebug() << "proxy" << d->socket->proxy().type() << d->socket->proxy().hostName() << d->socket->proxy().port();
		d->socket->connectToHost(d->host_name, d->port);
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

QAbstractSocket *DirectConnection::socket() const
{
	return d_func()->socket;
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
