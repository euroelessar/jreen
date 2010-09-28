/****************************************************************************
 *  abstractroster.h
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

// TODO: Create simple RosterManager for bots and simple applications

#ifndef ABSTRACTROSTER_H
#define ABSTRACTROSTER_H

#include "stanzaextension.h"
#include "presence.h"
#include "langmap.h"
#include "jid.h"
#include <QStringList>
#include <QHash>
#include <QSet>

template<typename T>
inline uint qHash(QSharedPointer<T> key) { return qHash(key.data()); }

namespace jreen
{

class Client;
class IQ;
class Presence;

#define J_ROSTER(RosterItem) \
public: \
	inline QHash<QString, QSharedPointer<RosterItem> > &items() \
	{ \
		return *reinterpret_cast<QHash<QString, QSharedPointer<RosterItem> > *>(&m_items); \
	} \
	inline QSharedPointer<RosterItem> item(const JID &jid) \
	{ \
		return m_items.value(jid.bare()).staticCast<RosterItem>(); \
	} \
	inline QSharedPointer<RosterItem> item(const QString &jid) \
	{ \
		return m_items.value(jid).staticCast<RosterItem>(); \
	} \
	inline const QHash<QString, QSharedPointer<RosterItem> > &items() const \
	{ \
		return *reinterpret_cast<const QHash<QString, QSharedPointer<RosterItem> > *>(&m_items); \
	} \
	inline const QSharedPointer<RosterItem> item(const JID &jid) const \
	{ \
		return m_items.value(jid.bare()).staticCast<RosterItem>(); \
	} \
	inline const QSharedPointer<RosterItem> item(const QString &jid) const \
	{ \
		return m_items.value(jid).staticCast<RosterItem>(); \
	} \
	inline const QSharedPointer<RosterItem> self() const \
	{ \
		return m_self.staticCast<RosterItem>(); \
	} \
	inline QSharedPointer<RosterItem> self() \
	{ \
		return m_self.staticCast<RosterItem>(); \
	} \
private:

#define J_ROSTER_ITEM(Resource) \
	inline QHash<QString, QSharedPointer<Resource> > &resources() \
	{ \
		return *reinterpret_cast<QHash<QString, QSharedPointer<Resource> > *>(&m_resources); \
	} \
	QSharedPointer<Resource> resource(const QString &resource) \
	{ \
		return m_resources.value(resource).staticCast<Resource>(); \
	} \
public: \
	inline const QHash<QString, QSharedPointer<Resource> > &resources() const \
	{ \
		return *reinterpret_cast<const QHash<QString, QSharedPointer<Resource> > *>(&m_resources); \
	} \
	const QSharedPointer<Resource> resource(const QString &resource) const \
	{ \
		return m_resources.value(resource).staticCast<Resource>(); \
	} \
private:

enum AbstractRosterIqContext
{
	SyncContext,
	LoadRoster,
	AddRosterItem,
	RemoveRosterItem
};

class AbstractRoster;

struct AbstractResourcePrivate
{
	inline AbstractResourcePrivate() : presence(QDomElement()) {}
	Presence presence;
	QString resource;
};

class JREEN_EXPORT AbstractResource
{
	Q_DECLARE_PRIVATE(AbstractResource)
	friend class AbstractRoster;
public:
	inline AbstractResource(AbstractResourcePrivate *data = 0) : d_ptr(data?data:new AbstractResourcePrivate) {}
	inline const QString &message(const QString &lang = QString()) const { return d_ptr->presence.status(lang); }
	inline int priority() const { return d_ptr->presence.priority(); }
	inline const QString &resource() const { return d_ptr->resource; }
	inline Presence::Type presence() const { return d_ptr->presence.subtype(); }
	inline const StanzaExtensionList &extensions() const { return d_ptr->presence.extensions(); }
protected:
	AbstractResourcePrivate	*d_ptr;
};

struct AbstractRosterItemPrivate;

class JREEN_EXPORT AbstractRosterItem
{
	J_ROSTER_ITEM(AbstractResource)
	Q_DECLARE_PRIVATE(AbstractRosterItem)
	friend class AbstractRoster;
	enum SubscriptionType
	{
		From,
		To,
		Both,
		Remove,
		None
	};
public:
	AbstractRosterItem(const QDomElement &node, AbstractRoster *roster, AbstractRosterItemPrivate *data = 0);
	virtual ~AbstractRosterItem();
	virtual QDomElement node(QDomDocument *doc) const;
	inline const QString &jid() const;
	inline const QString &name() const;
	inline const QStringList &groups() const;
	inline SubscriptionType subscriptionType() const;
	inline const QString &ask() const;
	inline void setGroups(const QStringList &groups);
	inline void setName(const QString &name);
protected:
	virtual void setData(const QSharedPointer<AbstractRosterItem> &item);
	inline void setChanged();
	QScopedPointer<AbstractRosterItemPrivate> d_ptr;
	AbstractRoster *m_roster;
	QHash<QString, QSharedPointer<AbstractResource> > m_resources;
};

struct AbstractRosterItemPrivate
{
	inline AbstractRosterItemPrivate() : subscription(AbstractRosterItem::Remove) {}
	QString jid;
	QString name;
	QStringList groups;
	AbstractRosterItem::SubscriptionType subscription;
	QString ask;
};

struct AbstractRosterPrivate
{
	Client *client;
};

class JREEN_EXPORT AbstractRoster : public QObject
{
	Q_OBJECT
	J_ROSTER(AbstractRosterItem)
	Q_DECLARE_PRIVATE(AbstractRoster)
public:
	AbstractRoster(Client *client, AbstractRosterPrivate *data = 0);
	virtual ~AbstractRoster() {}
public slots:
	virtual void load();
	virtual void synchronize();
protected:
	class Query : public StanzaExtension
	{
		J_PURE_EXTENSION(Query,"/iq/query[@xmlns='jabber:iq:roster']");
	public:
		Query(AbstractRoster *roster, const QDomElement &node = QDomElement());
		inline Query(QSharedPointer<AbstractRosterItem> item) : m_items(QList<QSharedPointer<AbstractRosterItem> >()<<item), m_roster(0) {}
		StanzaExtension *fromNode(const QDomElement &node) const { return new Query(m_roster, node); }
		QDomElement node(QDomDocument *document) const;
		inline const QList<QSharedPointer<AbstractRosterItem> > &items() const { return m_items; }
	protected:
		QList<QSharedPointer<AbstractRosterItem> > m_items;
		AbstractRoster *m_roster;
	};
	virtual QSharedPointer<AbstractRosterItem> createItem(const QDomElement &node = QDomElement());
	virtual QSharedPointer<AbstractResource> createResource();
	virtual void init();
	virtual void add(const JID &jid, const QString &name, const QStringList &groups = QStringList());
	virtual void remove(const JID &jid);
signals:
	void loaded();
	void selfPresence(QSharedPointer<AbstractRosterItem> item, const Presence &presence);
	void rosterPresence(QSharedPointer<AbstractRosterItem> item, const Presence &presence);
	void nonRosterPresence(const Presence &presence);
protected slots:
	virtual void handleIQ(const IQ &iq);
	virtual void handleIQ(const IQ &iq, int context);
	virtual void handlePresence(const Presence &presence);
protected:
	virtual void onItemAdded(QSharedPointer<AbstractRosterItem> item) { Q_UNUSED(item); }
	virtual void onItemUpdated(QSharedPointer<AbstractRosterItem> item) { Q_UNUSED(item); }
	virtual void onItemRemoved(const QString &jid) { Q_UNUSED(jid); }
	virtual void onLoaded(const QList<QSharedPointer<AbstractRosterItem> > &items);
	QScopedPointer<AbstractRosterPrivate> d_ptr;
	QSharedPointer<AbstractRosterItem> m_self;
	QHash<QString, QSharedPointer<AbstractRosterItem> > m_items;
	QSet<QSharedPointer<AbstractRosterItem> > m_changed_items;
	friend class Query;
	friend class AbstractRosterItem;
};

inline const QString &AbstractRosterItem::jid() const
{
	return d_ptr->jid;
}

inline const QString &AbstractRosterItem::name() const
{
	return d_ptr->name;
}

inline const QStringList &AbstractRosterItem::groups() const
{
	return d_ptr->groups;
}

inline AbstractRosterItem::SubscriptionType AbstractRosterItem::subscriptionType() const
{
	return d_ptr->subscription;
}

inline const QString &AbstractRosterItem::ask() const
{
	return d_ptr->ask;
}

inline void AbstractRosterItem::setGroups(const QStringList &groups)
{
	setChanged(); d_ptr->groups = groups;
}

inline void AbstractRosterItem::setName(const QString &name)
{
	setChanged(); d_ptr->name = name;
}

inline void AbstractRosterItem::setChanged()
{
	m_roster->m_changed_items << m_roster->m_items.value(d_ptr->jid);
}

}

#endif // ABSTRACTROSTER_H
