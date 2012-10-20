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

#ifndef MESSAGESESSION_H
#define MESSAGESESSION_H

#include "message.h"
#include <QPointer>
#include <QHash>
#include <QVector>
#include <QMap>

namespace Jreen
{

class MessageSession;
class MessageSessionManager;
class Client;

#define J_MESSAGE_FILTER(Class) \
	public:  \
		typedef QSharedPointer<Class> Ptr; \
		static int staticFilterType() \
		{ \
			static QBasicAtomicInt filterType = Q_BASIC_ATOMIC_INITIALIZER(0); \
			if (!filterType) { \
				filterType = Jreen::Payload::registerPayloadType( #Class ); \
				Class *useFullNameWithNamespaces = reinterpret_cast< ::Class* >(0); \
				Q_UNUSED(useFullNameWithNamespaces); \
			} \
			return filterType; \
		} \
		virtual int filterType() const \
		{ \
			Q_UNUSED(static_cast<const ::Class*>(this)); \
			return staticFilterType(); \
		} \
	private:

struct MessageFilterMeta
{
	MessageFilterMeta();
	const int type;
};

class JREEN_EXPORT MessageFilter
{
public:
	MessageFilter(MessageSession *session);
	virtual ~MessageFilter() {}
	virtual void filter(Message &message) = 0;
	virtual void decorate(Message &message) = 0;
	virtual void reset() = 0;
	virtual int filterType() const = 0;
protected:
	void send(const Message &message);
private:
	MessageSession *m_session;
};

class JREEN_EXPORT MessageSession : public QObject
{
	Q_OBJECT
	friend class MessageFilter;
public:
	MessageSession(MessageSessionManager *manager, const JID &jid, bool ignore_thread = true, const QString &thread = QString());
	virtual ~MessageSession();
	const QString &thread() { return m_thread; }
	const JID &jid() { return m_jid; }
	bool ignoreThread() { return m_ignore_thread; }
	void resetResource();
	void registerMessageFilter(MessageFilter *filter);
	template< typename T >
	Q_INLINE_TEMPLATE T messageFilter()
	{
		return static_cast<T>(m_filters.value(static_cast<T>(0).meta().type, 0));
	}
	virtual void sendMessage(const QString &body, const QString &subject = QString());
	void sendMessage(const Message &message);
	virtual void handleMessage(const Message &message);
signals:
	void messageReceived(const Jreen::Message &message);
	void jidChanged(const Jreen::JID &from, const Jreen::JID &to);
protected:
	virtual void send(const Message &message);
	void filter(Message &message);
	void decorate(Message &message);
	void setJid(const JID &jid);
	QString m_thread;
	MessageSessionManager *m_manager;
	bool m_ignore_thread;
	bool m_want_upgrade;
private:
	JID m_jid;
	QMap<int, MessageFilter *> m_filters;
};

class MessageSessionHandler
{
public:
	virtual ~MessageSessionHandler() {}
	virtual void handleMessageSession(MessageSession *session) = 0;
};

class MessageSessionManagerPrivate;
class JREEN_EXPORT MessageSessionManager : public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(MessageSessionManager)
public:
	MessageSessionManager(Client *client);
	virtual ~MessageSessionManager();
	void send(const Message &message);
	void registerMessageSession(MessageSession *session);
	void registerMessageSessionHandler(MessageSessionHandler *handler, QList<Message::Type> types);//WTF? O_o
	void removeMessageSessionHandler(MessageSessionHandler *handler);
	MessageSession *session(const JID &jid, Message::Type type, bool create = true);
public slots:
	virtual void handleMessage(const Jreen::Message &message);
signals:
	void messageReceived(const Jreen::Message &message);
	void sessionCreated(Jreen::MessageSession *session);
private:
	QScopedPointer<MessageSessionManagerPrivate> d_ptr;
	friend class MessageSession;
};

}

#endif // MESSAGESESSION_H
