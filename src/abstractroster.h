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

//template<typename T>
//inline uint qHash(QSharedPointer<T> key) { return qHash(key.data()); }

namespace jreen
{

class Client;
class IQ;
class Presence;
class AbstractRoster;
class AbstractRosterPrivate;
class AbstractRosterItem;
class AbstractRosterItemPrivate;
class AbstractRosterQuery;

class ItemFactory : public XmlStreamFactory<AbstractRosterItem>
{
public:
};

/* #define J_ROSTER(RosterItem) \
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
*/

/* #define J_ROSTER_ITEM(Resource) \
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
private:*/

enum AbstractRosterIqContext
{
	SyncContext,
	LoadRoster,
	AddRosterItem,
	RemoveRosterItem
};

//struct AbstractResourcePrivate
//{
//	inline AbstractResourcePrivate() : presence(QDomElement()) {}
//	Presence presence;
//	QString resource;
//};

//class JREEN_EXPORT AbstractResource
//{
//	Q_DECLARE_PRIVATE(AbstractResource)
//	friend class AbstractRoster;
//public:
//	inline AbstractResource(AbstractResourcePrivate *data = 0) : d_ptr(data?data:new AbstractResourcePrivate) {}
//	inline const QString &message(const QString &lang = QString()) const { return d_ptr->presence.status(lang); }
//	inline int priority() const { return d_ptr->presence.priority(); }
//	inline const QString &resource() const { return d_ptr->resource; }
//	inline Presence::Type presence() const { return d_ptr->presence.subtype(); }
//	inline const StanzaExtensionList &extensions() const { return d_ptr->presence.extensions(); }
//protected:
//	AbstractResourcePrivate	*d_ptr;
//};

class AbstractRosterItemPrivate;

class JREEN_EXPORT AbstractRosterItem
{
	//	J_ROSTER_ITEM(AbstractResource)
	Q_DECLARE_PRIVATE(AbstractRosterItem)
	friend class AbstractRoster;
public:
	enum SubscriptionType
	{
		From,
		To,
		Both,
		Remove,
		None,
		Invalid = -1
	};
	typedef QSharedPointer<AbstractRosterItem> Ptr;
	AbstractRosterItem(AbstractRoster *roster, AbstractRosterItemPrivate *data = 0);
	virtual ~AbstractRosterItem();
	const QString &jid() const;
	const QString &name() const;
	const QStringList &groups() const;
	SubscriptionType subscriptionType() const;
	const QString &ask() const;
	void setGroups(const QStringList &groups);
	void setName(const QString &name);
protected:
	virtual void setData(const QSharedPointer<AbstractRosterItem> &item);
	void setChanged();
	QScopedPointer<AbstractRosterItemPrivate> d_ptr;
	//	QHash<QString, QSharedPointer<AbstractResource> > m_resources;
};

//struct AbstractRosterItemPrivate
//{
//	inline AbstractRosterItemPrivate() : subscription(AbstractRosterItem::Remove) {}
//	QString jid;
//	QString name;
//	QStringList groups;
//	AbstractRosterItem::SubscriptionType subscription;
//	QString ask;
//};

//struct AbstractRosterPrivate
//{
//	Client *client;
//};

class JREEN_EXPORT AbstractRoster : public QObject
{
	Q_OBJECT
	//	J_ROSTER(AbstractRosterItem)
	Q_DECLARE_PRIVATE(AbstractRoster)
public:
	AbstractRoster(Client *client, AbstractRosterPrivate *data = 0);
	virtual ~AbstractRoster();	
public slots:
	virtual void load();
	virtual void synchronize();
protected:
	virtual QSharedPointer<AbstractRosterItem> createItem();
	virtual QSharedPointer<AbstractRosterItem> getItem(const JID &jid) const;
	//	virtual QSharedPointer<AbstractResource> createResource();
	virtual void init();
	virtual void add(const JID &jid, const QString &name, const QStringList &groups = QStringList());
	virtual void remove(const JID &jid);
signals:
	void loaded();
	//	void selfPresence(QSharedPointer<AbstractRosterItem> item, const Presence &presence);
	//	void rosterPresence(QSharedPointer<AbstractRosterItem> item, const Presence &presence);
	//	void nonRosterPresence(const Presence &presence);
protected slots:
	virtual void handleIQ(const jreen::IQ &iq);
	virtual void handleIQ(const jreen::IQ &iq, int context);
//	virtual void handlePresence(const jreen::Presence &presence);
protected:
	virtual void onItemAdded(QSharedPointer<AbstractRosterItem> item) { Q_UNUSED(item); }
	virtual void onItemUpdated(QSharedPointer<AbstractRosterItem> item) { Q_UNUSED(item); }
	virtual void onItemRemoved(const QString &jid) { Q_UNUSED(jid); }
	virtual void onLoaded(const QList<QSharedPointer<AbstractRosterItem> > &items);
	QScopedPointer<AbstractRosterPrivate> d_ptr;
	QSharedPointer<AbstractRosterItem> m_self;
	QHash<QString, QSharedPointer<AbstractRosterItem> > m_items;
	QSet<QSharedPointer<AbstractRosterItem> > m_changed_items;
	friend class AbstractRosterQuery;
	friend class AbstractRosterQueryFactory;
	friend class AbstractRosterItem;
};

}

#endif // ABSTRACTROSTER_H
