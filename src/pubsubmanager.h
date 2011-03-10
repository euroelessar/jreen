/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2011 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#ifndef PUBSUBMANAGER_H
#define PUBSUBMANAGER_H

#include "pubsubevent.h"

namespace Jreen
{
class JID;
class Message;

namespace PubSub
{
class PublishOptionsPrivate;
class PublishOptions
{
public:
	enum AccessModel
	{
		AuthorizeAccess,
		OpenAccess,
		PresenceAccess,
		RosterAccess,
		WhitelistAccess
	};

	PublishOptions();
	PublishOptions(const PublishOptions &o);
	PublishOptions &operator =(const PublishOptions &o);
	~PublishOptions();

	AccessModel accessModel() const;
	void setAccessModel(AccessModel model);
	bool isPersistent() const;
	void setPersistent(bool persistent);
private:
	QExplicitlySharedDataPointer<PublishOptionsPrivate> d_ptr;
};

class ManagerPrivate;
class JREEN_EXPORT Manager : public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(Manager)
public:
	Manager(Client *client);
	virtual ~Manager();

	void publishItems(const QList<StanzaExtension::Ptr> &items, const JID &to);
	void publishItems(const QList<StanzaExtension::Ptr> &items, const JID &to,
					  const PublishOptions &options);

	template <typename T>
	void addEntityType(T *entity = 0) { addEntityType(entity->staticExtensionType()); }
	void addEntityType(int type);
protected slots:
	void handleMessage(const Jreen::Message &message);
signals:
	void eventReceived(const Jreen::PubSub::Event::Ptr &event, const Jreen::JID &from);
private:
	QScopedPointer<ManagerPrivate> d_ptr;
};
}
}

#endif // PUBSUBMANAGER_H
