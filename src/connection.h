/****************************************************************************
**
** Jreen
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef CONNECTION_H
#define CONNECTION_H

#include <QIODevice>
#include "jreen.h"

class QNetworkProxy;
class QAuthenticator;

namespace Jreen
{

// Why there is no Qt 5 today?..
class JREEN_EXPORT Connection : public QIODevice
{
	Q_OBJECT
public:
	enum SocketError {
		ConnectionRefusedError,
		RemoteHostClosedError,
		HostNotFoundError,
		SocketAccessError,
		SocketResourceError,
		SocketTimeoutError,                     /* 5 */
		DatagramTooLargeError,
		NetworkError,
		AddressInUseError,
		SocketAddressNotAvailableError,
		UnsupportedSocketOperationError,        /* 10 */
		UnfinishedSocketOperationError,
		ProxyAuthenticationRequiredError,
		SslHandshakeFailedError,
		ProxyConnectionRefusedError,
		ProxyConnectionClosedError,             /* 15 */
		ProxyConnectionTimeoutError,
		ProxyNotFoundError,
		ProxyProtocolError,

		UnknownSocketError = -1
	};
	enum SocketState {
		UnconnectedState,
		HostLookupState,
		ConnectingState,
		ConnectedState,
		BoundState,
		ListeningState,
		ClosingState
	};
	virtual ~Connection() {}

	virtual SocketState socketState() const = 0;
	virtual SocketError socketError() const = 0;
	virtual bool open() = 0;
	virtual void close() = 0;

signals:
	void hostFound();
	void connected();
	void disconnected();
	void stateChanged(Jreen::Connection::SocketState);
	void error(Jreen::Connection::SocketError);

protected:
	virtual qint64 readData(char *data, qint64 maxlen) = 0;
	virtual qint64 writeData(const char *data, qint64 len) = 0;
private:
	bool open(OpenMode) { return false; }
};

class JREEN_EXPORT ConnectionListener : public QObject
{
	Q_OBJECT
public:
	virtual ~ConnectionListener() {}
	virtual bool hasPendingConnections () const = 0;
	virtual Connection *nextPendingConnection() = 0;
signals:
	void newConnection();
};

}

#endif // CONNECTION_H
