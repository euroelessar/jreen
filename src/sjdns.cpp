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

#include "sjdns_p.h"
#include "logger.h"

namespace Jreen
{

SJDns &SJDns::instance()
{
	static SJDns *sjdns = 0;
	if (sjdns && !sjdns->m_valid) {
		delete sjdns;
		sjdns = 0;
	}
	if (!sjdns) {
		sjdns = new SJDns;
		sjdns->m_qjdns = new QJDns;
		sjdns->m_valid = true;
		if (!sjdns->m_qjdns->init(QJDns::Unicast, QHostAddress::Any)) {
			delete sjdns->m_qjdns;
			sjdns->m_qjdns = 0;
			sjdns->m_valid = false;
			return *sjdns;
		}
		connect(sjdns->m_qjdns, SIGNAL(resultsReady(int,QJDns::Response)), sjdns, SLOT(resultsReady(int,QJDns::Response)));
		connect(sjdns->m_qjdns, SIGNAL(published(int)), sjdns, SLOT(published(int)));
		connect(sjdns->m_qjdns, SIGNAL(error(int,QJDns::Error)), sjdns, SLOT(error(int,QJDns::Error)));

		QJDns::SystemInfo info = QJDns::systemInfo();
		if (info.nameServers.isEmpty()) {
			QJDns::NameServer server;
			server.address = QLatin1String("8.8.8.8");
			info.nameServers << server;
			server.address = QLatin1String("77.88.39.152");
			info.nameServers << server;
		}
		sjdns->m_qjdns->setNameServers(info.nameServers);
	}
	return *sjdns;
}

bool SJDns::isValid()
{
	return m_valid;
}

void SJDns::doLookup(const QString &host, QObject *receiver, const char *member)
{
	Q_ASSERT(m_valid);
	int id = m_qjdns->queryStart(host.toLatin1(), QJDns::Srv);
	Action *action = new Action(this);
	action->setData(host);
	connect(action, SIGNAL(triggered()), receiver, member);
	m_actions.insert(id, action);
}

const QJDns::Response *SJDns::servers(const QString &host)
{
	Q_ASSERT(m_valid);
	QHash<QString, QJDns::Response>::const_iterator iter = m_results.constFind(host);
	if(iter == m_results.constEnd())
		return 0;
	return &iter.value();
}

void SJDns::resultsReady(int id, const QJDns::Response &results)
{
	Action *action = m_actions.value(id, 0);
	Q_ASSERT(action);
	foreach(const QJDns::Record &record, results.answerRecords)
		jreenDebug() << action->data() << record.name << record.port << record.priority << record.weight;
	m_results.insert(action->data().toString(), results);
	action->trigger();
}

void SJDns::published(int id)
{
	Q_UNUSED(id);
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
	switch (e) {
	case QJDns::ErrorGeneric:
		jreenCritical() << "error Generic" << id;
		break;
	case QJDns::ErrorNXDomain:
		jreenCritical() << "error NXDomain" << id;
		break;
	case QJDns::ErrorTimeout:
		jreenCritical() << "error Timeout" << id;
		break;
	case QJDns::ErrorConflict:
		jreenCritical() << "error Conflict" << id;
		break;
	}
}

DnsLookup::DnsLookup(QObject *parent)
	: QObject(parent), m_type(QJDns::Any), m_response(NULL)
{
}

DnsLookup::~DnsLookup()
{
}

void DnsLookup::setType(DnsLookup::Type type)
{
	m_type = static_cast<QJDns::Type>(type);
}

void DnsLookup::setName(const QString &name)
{
	m_name = name;
}

void DnsLookup::lookup()
{
	if (SJDns::instance().isValid())
		SJDns::instance().doLookup(m_name, this, SLOT(onResultReady()));
	else
		emit finished();
}

QList<DnsServiceRecord> DnsLookup::serviceRecords() const
{
	QList<DnsServiceRecord> result;
	if (!m_response)
		return result;

	foreach(const QJDns::Record &qrecord, m_response->answerRecords)	{
		DnsServiceRecord record;
		record.m_target = QUrl::fromAce(qrecord.name);
		record.m_port = qrecord.port;
		record.m_weight = qrecord.weight;
		record.m_priority = qrecord.priority;
		result << record;
	}
	return result;
}

DnsLookup::Error DnsLookup::error() const
{
	return m_response ? NoError : SomeError;
}

void DnsLookup::onResultReady()
{
	m_response = SJDns::instance().servers(m_name);
	emit finished();
}

}
