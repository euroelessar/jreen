/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#ifndef ABSTRACTROSTER_P_H
#define ABSTRACTROSTER_P_H

#include "abstractroster.h"

namespace jreen
{
class AbstractRosterItemPrivate
{
public:
	inline AbstractRosterItemPrivate() : subscription(AbstractRosterItem::Remove) {}
	AbstractRoster *roster;
	QString jid;
	QString name;
	QStringList groups;
	AbstractRosterItem::SubscriptionType subscription;
	QString ask;

	static AbstractRosterItemPrivate *get(AbstractRosterItem *item) { return item->d_func(); }
};

class AbstractRosterPrivate
{
public:
	Client *client;
};

class AbstractRosterQuery : public StanzaExtension
{
	J_EXTENSION(jreen::AbstractRosterQuery,"/iq/query[@xmlns='jabber:iq:roster']");
public:
	AbstractRosterQuery(const QString &ver = QString()) : m_ver(ver) {}
	AbstractRosterQuery(const QList<AbstractRosterItem::Ptr> &items) : m_items(items) {}
	AbstractRosterQuery(const QSharedPointer<AbstractRosterItem> &item) { m_items << item; }
	QList<QSharedPointer<AbstractRosterItem> > items() const { return m_items; }
	QString ver() const { return m_ver; }
protected:
	QString m_ver;
	QList<QSharedPointer<AbstractRosterItem> > m_items;
};

class AbstractRosterQueryFactory : public StanzaExtensionFactory<AbstractRosterQuery>
{
public:
	AbstractRosterQueryFactory(AbstractRoster *roster);
	QStringList features() const;
	bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleEndElement(const QStringRef &name, const QStringRef &uri);
	void handleCharacterData(const QStringRef &text);
	void serialize(StanzaExtension *extension, QXmlStreamWriter *writer);
	StanzaExtension::Ptr createExtension();
private:
	enum State { AtStart, AtQuery, AtItem, AtGroup };
	int m_depth;
	State m_state;
	AbstractRoster *m_roster;
	QStringList m_groups;
	QString m_name;
	QString m_jid;
	AbstractRosterItem::SubscriptionType m_subscription;
	QString m_ver;
	QList<AbstractRosterItem::Ptr> m_items;
};
}

#endif // ABSTRACTROSTER_P_H
