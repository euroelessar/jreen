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

#ifndef ABSTRACTROSTER_P_H
#define ABSTRACTROSTER_P_H

#include "abstractroster.h"

namespace Jreen
{
class RosterItemPrivate
{
public:
	inline RosterItemPrivate() : subscription(RosterItem::Remove) {}
	AbstractRoster *roster;
	QString jid;
	QString name;
	QStringList groups;
	RosterItem::SubscriptionType subscription;
	QString ask;

	static RosterItemPrivate *get(RosterItem *item) { return item->d_func(); }
};

class AbstractRosterPrivate
{
public:
	Client *client;
	QString version;
	QSharedPointer<RosterItem> self;
	QHash<QString, QSharedPointer<RosterItem> > items;
	QSet<QSharedPointer<RosterItem> > changed_items;
};

class AbstractRosterQuery : public Payload
{
	J_PAYLOAD(Jreen::AbstractRosterQuery);
public:
	AbstractRosterQuery(const QString &ver = QString()) : m_ver(ver) {}
	AbstractRosterQuery(const QList<RosterItem::Ptr> &items, const QString &ver) : m_items(items), m_ver(ver) {}
	AbstractRosterQuery(const QSharedPointer<RosterItem> &item) { m_items << item; }
	QList<QSharedPointer<RosterItem> > items() const { return m_items; }
	QString ver() const { return m_ver; }
protected:
	QList<QSharedPointer<RosterItem> > m_items;
	QString m_ver;
};

class JREEN_AUTOTEST_EXPORT AbstractRosterQueryFactory : public PayloadFactory<AbstractRosterQuery>
{
public:
	AbstractRosterQueryFactory(AbstractRoster *roster);
	QStringList features() const;
	bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleEndElement(const QStringRef &name, const QStringRef &uri);
	void handleCharacterData(const QStringRef &text);
	void serialize(Payload *extension, QXmlStreamWriter *writer);
	Payload::Ptr createPayload();
private:
	enum State { AtStart, AtQuery, AtItem, AtGroup };
	int m_depth;
	State m_state;
	AbstractRoster *m_roster;
	QStringList m_groups;
	QString m_name;
	QString m_jid;
	QString m_ask;
	RosterItem::SubscriptionType m_subscription;
	QString m_ver;
	QList<RosterItem::Ptr> m_items;
};
}

#endif // ABSTRACTROSTER_P_H
