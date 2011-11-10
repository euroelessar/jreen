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

#include "sjdns_p.h"

namespace Jreen
{

SJDns &SJDns::instance()
{
	static SJDns *sjdns = 0;
	if(!sjdns)
	{
		sjdns = new SJDns;
		sjdns->qjdns = new QJDns;
		sjdns->qjdns->init(QJDns::Unicast, QHostAddress::Any);
		QJDns::NameServer host;
		connect(sjdns->qjdns, SIGNAL(resultsReady(int,QJDns::Response)), sjdns, SLOT(resultsReady(int,QJDns::Response)));
		connect(sjdns->qjdns, SIGNAL(published(int)), sjdns, SLOT(published(int)));
		connect(sjdns->qjdns, SIGNAL(error(int,QJDns::Error)), sjdns, SLOT(error(int,QJDns::Error)));

		QJDns::SystemInfo info = QJDns::systemInfo();
		if (info.nameServers.isEmpty()) {
			QJDns::NameServer server;
			server.address = QLatin1String("8.8.8.8");
			info.nameServers << server;
			server.address = QLatin1String("77.88.39.152");
			info.nameServers << server;
		}
		sjdns->qjdns->setNameServers(info.nameServers);
	}
	return *sjdns;
}

const QJDns::Response *SJDns::servers(const QString &host)
{
	QHash<QString, QJDns::Response>::const_iterator iter = m_results.find(host);
	if(iter == m_results.constEnd())
		return 0;
	return &iter.value();
}

void SJDns::resultsReady(int id, const QJDns::Response &results)
{
	Action *action = m_actions.value(id, 0);
	Q_ASSERT(action);
	foreach(const QJDns::Record &record, results.answerRecords)
		qDebug() << record.name << record.port << record.priority << record.weight;
	m_results.insert(action->data().toString(), results);
	action->trigger();
}

void SJDns::error(int id, QJDns::Error e)
{
	Action *action = m_actions.value(id, 0);
	Q_ASSERT(action);
	QJDns::Response response;
	QJDns::Record record;
	record.port = 5222;
	record.name = action->data().toString().toUtf8();
	response.answerRecords << record;
	m_results.insert(record.name, response);
	action->trigger();
	switch(e)
	{
	case QJDns::ErrorGeneric:
		qDebug("error %s %d", "Generic", id);
		break;
	case QJDns::ErrorNXDomain:
		qDebug("error %s %d", "NXDomain", id);
		break;
	case QJDns::ErrorTimeout:
		qDebug("error %s %d", "Timeout", id);
		break;
	case QJDns::ErrorConflict:
		qDebug("error %s %d", "Conflict", id);
		break;
	}
}

}
