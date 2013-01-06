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

// TODO: Create simple RosterManager for bots and simple applications

#ifndef ABSTRACTROSTER_H
#define ABSTRACTROSTER_H

#include <QSharedPointer>

#if QT_VERSION < QT_VERSION_CHECK(4, 7, 0)
// Looks like this function was overloaded at this release, if not - tell me // EuroElessar
template <class T> inline uint qHash(const T *key);
template<typename T>
inline uint qHash(const QSharedPointer<T> &key) { return qHash(key.data()); }
#endif

#include "stanzaextension.h"
#include "presence.h"
#include "langmap.h"
#include "jid.h"
#include <QStringList>
#include <QHash>
#include <QSet>

namespace Jreen
{

class Client;
class IQ;
class Presence;
class AbstractRoster;
class AbstractRosterPrivate;
class SimpleRosterPrivate;
class RosterItem;
class RosterItemPrivate;
class AbstractRosterQuery;

class JREEN_AUTOTEST_EXPORT ItemFactory : public XmlStreamFactory<RosterItem>
{
public:
};

enum AbstractRosterIqContext
{
	SyncContext,
	LoadRoster,
	AddRosterItem,
	RemoveRosterItem
};

class RosterItemPrivate;

class JREEN_EXPORT RosterItem
{
	Q_DECLARE_PRIVATE(RosterItem)
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
	typedef QSharedPointer<RosterItem> Ptr;
	RosterItem(const QString &jid, const QString &name,
					   const QStringList &groups, SubscriptionType s10n);
	RosterItem(AbstractRoster *roster, RosterItemPrivate *data = 0);
	virtual ~RosterItem();
	QString jid() const;
	QString name() const;
	QStringList groups() const;
	SubscriptionType subscription() const;
	QString ask() const;
	void setGroups(const QStringList &groups);
	void setName(const QString &name);
protected:
	virtual void setData(const QSharedPointer<RosterItem> &item);
	void setChanged();
	QScopedPointer<RosterItemPrivate> d_ptr;
};

class JREEN_EXPORT AbstractRoster : public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(AbstractRoster)
public:
	AbstractRoster(Client *client, AbstractRosterPrivate *data = 0);
	virtual ~AbstractRoster();	
	
	QString version() const;
	RosterItem::Ptr item(const JID &jid) const;
	RosterItem::Ptr selfItem() const;
	void fillRoster(const QString &version, const QList<RosterItem::Ptr> &items);
public slots:
	virtual void load();
	virtual void synchronize();
protected:
	virtual QSharedPointer<RosterItem> createItem();
	virtual void init();
	virtual void add(const JID &jid, const QString &name = QString(), const QStringList &groups = QStringList());
	virtual void remove(const JID &jid);
signals:
	void loaded();
protected slots:
	virtual void handleIQ(const Jreen::IQ &iq);
	virtual void handleIQ(const Jreen::IQ &iq, int context);
protected:
	virtual void onItemAdded(QSharedPointer<RosterItem> item);
	virtual void onItemUpdated(QSharedPointer<RosterItem> item);
	virtual void onItemRemoved(const QString &jid);
	virtual void onLoaded(const QList<QSharedPointer<RosterItem> > &items);
	QScopedPointer<AbstractRosterPrivate> d_ptr;
	friend class AbstractRosterQuery;
	friend class JREEN_AUTOTEST_EXPORT AbstractRosterQueryFactory;
	friend class RosterItem;
};

class JREEN_EXPORT SimpleRoster : public AbstractRoster
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(SimpleRoster)
public:
	SimpleRoster(Client *client, SimpleRosterPrivate *data = 0);
	virtual ~SimpleRoster();	
	
	void subscribe(const JID &jid, const QString &msg, const QString &name, const QStringList &groups);
	void subscribe(const JID &jid, const QString &msg = QString());
	void unsubscribe(const JID &jid, const QString &msg = QString());
	void allowSubscription(const JID &jid, bool ok);
	using AbstractRoster::add;
	using AbstractRoster::remove;
signals:
	void itemAdded(const Jreen::RosterItem::Ptr &item);
	void itemUpdated(const Jreen::RosterItem::Ptr &item);
	void itemRemoved(const QString &jid);
	void presenceReceived(const Jreen::RosterItem::Ptr &item, const Jreen::Presence &presence);
	void subscriptionReceived(const Jreen::RosterItem::Ptr &item, const Jreen::Presence &presence);
protected slots:
	void onPresenceReceived(const Jreen::Presence &presence);
protected:
	virtual void onItemAdded(QSharedPointer<RosterItem> item);
	virtual void onItemUpdated(QSharedPointer<RosterItem> item);
	virtual void onItemRemoved(const QString &jid);
};

}

#endif // ABSTRACTROSTER_H
