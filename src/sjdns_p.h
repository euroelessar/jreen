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

//#include <QAction>
#include <jdns/qjdns.h>
#include <QUrl>
#include "action_p.h"

namespace Jreen
{

//class JAction;

// TODO: Make DNS API shared

class SJDns : QObject
{
	Q_OBJECT
	QJDns *m_qjdns;
	bool m_valid;
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
	QMap<int, Action *> m_actions;
	QHash<QString, QJDns::Response> m_results;
};

}

#endif // QJDNS_P_H
