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

#ifndef JREEN_DIRECTCONNECTION_P_H
#define JREEN_DIRECTCONNECTION_P_H

#include <QSslSocket>
#include <QHostAddress>
#include <QNetworkProxy>
#include "jreen.h"
#include "directconnection.h"
#include "logger.h"
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
	
	DirectConnectionPrivate(const QString &hn, int p, DirectConnection *par);
	void connectSocket();
	void doLookup();
	
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
	void lookupResultsReady();
	void stateChanged(QAbstractSocket::SocketState ss);
	void error(QAbstractSocket::SocketError se);
};

}

#endif // JREEN_DIRECTCONNECTION_P_H
