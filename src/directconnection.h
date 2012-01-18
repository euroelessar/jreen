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

#ifndef DIRECTCONNECTION_H
#define DIRECTCONNECTION_H

#include "connection.h"

class QAbstractSocket;
class QHostAddress;

namespace Jreen
{

class DirectConnectionPrivate;

class JREEN_EXPORT DirectConnection : public Connection
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(DirectConnection)

public:
	~DirectConnection();

	bool open();
	void close();
	void setHost(const QHostAddress &host);
	void setHost(const QString &host);
	void setPort(int port);
	void setProxy(const QNetworkProxy &proxy);
	qint64 bytesAvailable() const;
	SocketState socketState() const;
	SocketError socketError() const;

signals:
	void proxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *authenticator);

protected:
	DirectConnection(QAbstractSocket *socket, const QString &host_name, qint16 port = -1);
	DirectConnection(QAbstractSocket *socket, const QHostAddress &address, qint16 port);
	qint64 readData(char *data, qint64 maxlen);
	qint64 writeData(const char *data, qint64 len);
	QScopedPointer<DirectConnectionPrivate> d_ptr;
};

//typedef DirectConnection<QTcpSocket> TcpConnection;
//typedef DirectConnection<QUdpSocket> UdpConnection;

}

#endif // DIRECTCONNECTION_H
