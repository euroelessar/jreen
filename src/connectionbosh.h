/****************************************************************************
**
** Jreen
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $JREEN_BEGIN_LICENSE$
** Jreen is free software: you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** Jreen is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with Jreen.  If not, see <http://www.gnu.org/licenses/>.
** $JREEN_END_LICENSE$
**
****************************************************************************/

#ifndef JREEN_CONNECTIONBOSH_H
#define JREEN_CONNECTIONBOSH_H

#include "connection.h"
#include "streamprocessor.h"

class QNetworkReply;

namespace Jreen
{
class ConnectionBOSHPrivate;
class JREEN_EXPORT ConnectionBOSH : public Connection, public StreamProcessor
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(ConnectionBOSH)
	Q_INTERFACES(Jreen::StreamProcessor)
public:
	ConnectionBOSH(const QString &host, int port = 5280);
	~ConnectionBOSH();

	bool open();
	void close();
	qint64 bytesAvailable() const;
	SocketState socketState() const;
	SocketError socketError() const;
protected:
	void authorized();
	QString sessionID() const;
	void setJID(const JID &jid);
	void setStreamParser(XmlStreamParser *parser);
	void restartStream();
	qint64 readData(char *data, qint64 maxlen);
	qint64 writeData(const char *data, qint64 len);
protected slots:
	void onRequestFinished(QNetworkReply *);
private:
	QScopedPointer<ConnectionBOSHPrivate> d_ptr;
};
}

#endif // JREEN_CONNECTIONBOSH_H
