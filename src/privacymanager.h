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

#ifndef PRIVACYMANAGER_H
#define PRIVACYMANAGER_H

#include "jid.h"

namespace Jreen
{
class Client;
class IQ;
class RosterItem;
class PrivacyManagerPrivate;
class PrivacyItemPrivate;

class JREEN_EXPORT PrivacyItem
{
public:
	enum Type
	{
		ByJID,
		ByGroup,
		BySubscription,
		All = -1
	};
	// Keep in sync with AbstractRosterItem::SubscriptionType
	enum SubscriptionType
	{
		From = 0,
		To,
		Both,
		None = 4,
		Invalid = -1
	};
	enum Action
	{
		Deny,
		Allow
	};
	enum StanzaType
	{
		Message     = 0x01,
		PresenceIn  = 0x02,
		PresenceOut = 0x04,
		IQ          = 0x08,
		AllStanzas  = 0x00
	};
	typedef QFlags<StanzaType> StanzaTypes;
	
	PrivacyItem();
	PrivacyItem(const PrivacyItem &o);
	PrivacyItem &operator =(const PrivacyItem &o);
	~PrivacyItem();
	
	Type type() const;
	JID jid() const;
	void setJID(const JID &jid);
	SubscriptionType subscription() const;
	void setSubscription(SubscriptionType type);
	QString group() const;
	void setGroup(const QString &group);
	Action action() const;
	void setAction(Action action);
	StanzaTypes stanzaTypes() const;
	void setStanzaTypes(StanzaTypes types);
	uint order() const;
	void setOrder(uint order);
	
	bool check(const RosterItem *item) const;
	bool check(const JID &jid) const;
private:
	QSharedDataPointer<PrivacyItemPrivate> d_ptr;
};

class JREEN_EXPORT PrivacyManager : public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(PrivacyManager)
public:
    PrivacyManager(Client *client);
	~PrivacyManager();
	
	QString defaultList() const;
	QString activeList() const;
	void desetActiveList();
	void setActiveList(const QString &name);
	void setDefaultList(const QString &name);
	QStringList lists() const;
public slots:
	void request();
	void requestList(const QString &name);
	void setList(const QString &name, const QList<Jreen::PrivacyItem> &list);
	void removeList(const QString &name);
private slots:
	void handleIQ(const Jreen::IQ &iq);
	void handleIQ(const Jreen::IQ &iq, int context);
signals:
	void activeListChanged(const QString &name);
	void defaultListChanged(const QString &name);
	void listsReceived();
	void listReceived(const QString &name, const QList<Jreen::PrivacyItem> &list);
	void listChanged(const QString &name);
private:
	QScopedPointer<PrivacyManagerPrivate> d_ptr;
};
}

#endif // PRIVACYMANAGER_H
