/****************************************************************************
**
** Jreen
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $JREEN_BEGIN_LICENSE$
** Jreen is free software: you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** Jreen is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with Jreen.  If not, see <http://www.gnu.org/licenses/>.
** $JREEN_END_LICENSE$
**
****************************************************************************/

#ifndef JREEN_PUBSUBMANAGER_H
#define JREEN_PUBSUBMANAGER_H

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

	void publishItems(const QList<Payload::Ptr> &items, const JID &to);
	void publishItems(const QList<Payload::Ptr> &items, const JID &to,
					  const PublishOptions &options);

	template <typename T>
	void addEntityType(T *entity = 0) { addEntityType(entity->staticPayloadType()); }
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

#endif // JREEN_PUBSUBMANAGER_H
