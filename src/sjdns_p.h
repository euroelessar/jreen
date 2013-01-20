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

#ifndef QJDNS_P_H
#define QJDNS_P_H

#include <qglobal.h>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#  error Check build scripts
#endif

#include <jdns/qjdns.h>
#include <QUrl>
#include "action_p.h"

namespace Jreen
{

class SJDns : public QObject
{
	Q_OBJECT
	SJDns() {}
	~SJDns() {}
public:
	static SJDns &instance();
	bool isValid();
	void doLookup(const QString &host, QObject *receiver, const char *member);
	const QJDns::Response *servers(const QString &host);

private slots:
	void resultsReady(int id, const QJDns::Response &results);
	void published(int id);
	void error(int id, QJDns::Error e);
private:
	QJDns *m_qjdns;
	bool m_valid;
	QMap<int, Action *> m_actions;
	QHash<QString, QJDns::Response> m_results;
};

class DnsServiceRecord
{
public:
	inline QString target() const { return m_target; }
	inline quint16 port() const { return m_port; }
	inline quint16 priority() const { return m_priority; }
	inline quint16 weight() const { return m_weight; }

	QString m_target;
	quint16 m_port;
	quint16 m_priority;
	quint16 m_weight;
};

class DnsLookup : public QObject
{
	Q_OBJECT
public:
	enum Error {
		NoError,
		SomeError
	};

	enum Type {
		Invalid = -1,
		SRV = QJDns::Srv
	};

	DnsLookup(QObject *parent);
	~DnsLookup();

	void setType(Type type);
	void setName(const QString &name);
	void lookup();

	QList<DnsServiceRecord> serviceRecords() const;
	Error error() const;

signals:
	void finished();

private slots:
	void onResultReady();

private:
	QJDns::Type m_type;
	QString m_name;
	const QJDns::Response *m_response;
};

typedef DnsLookup QDnsLookup;
typedef DnsServiceRecord QDnsServiceRecord;
//#endif

}

#endif // QJDNS_P_H
