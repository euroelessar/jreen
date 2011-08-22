/****************************************************************************
 *  sjdns_p.h
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#ifndef QJDNS_P_H
#define QJDNS_P_H

//#include <QAction>
#include "../3rdparty/jdns/qjdns.h"
#include <QUrl>
#include "action.h"

namespace Jreen
{

//class JAction;

// TODO: Make DNS API shared

class SJDns : QObject
{
	Q_OBJECT
	QJDns *qjdns;
	SJDns() {}
	~SJDns() {}
public:
	static SJDns &instance();
	void doLookup(const QString &host, QObject *receiver, const char *member)
	{
		int id = qjdns->queryStart("_xmpp-client._tcp." + QUrl::toAce(host), QJDns::Srv);
		Action *action = new Action(this);
		action->setData(host);
		connect(action, SIGNAL(triggered()), receiver, member);
		m_actions.insert(id, action);
	}
	const QJDns::Response *servers(const QString &host);
private slots:
	void resultsReady(int id, const QJDns::Response &results);
	void published(int id)
	{
		Q_UNUSED(id);
	}
	void error(int id, QJDns::Error e);
private:
	QMap<int, Action *> m_actions;
	QHash<QString, QJDns::Response> m_results;
};

}

#endif // QJDNS_P_H
