/****************************************************************************
 *  messagesession.h
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

#ifndef MESSAGESESSION_H
#define MESSAGESESSION_H

#include "message.h"
#include <QPointer>
#include <QHash>
#include <QVector>
#include <QMap>

namespace jreen
{

class MessageSession;
class MessageSessionManager;
class Client;

#define J_MESSAGE_FILTER \
	public: \
		inline static const MessageFilterMeta &meta() \
		{ \
			static MessageFilterMeta staticFilterMeta; \
			return staticFilterMeta; \
		} \
		virtual int filterType() const { return meta().type; } \
	private:

struct MessageFilterMeta
{
	MessageFilterMeta();
	const int type;
};

class JREEN_EXPORT MessageFilter
{
public:
	inline MessageFilter(MessageSession *session);
	virtual ~MessageFilter() {}
	virtual void filter(Message &message) = 0;
	virtual void decorate(Message &message) = 0;
	virtual void reset() = 0;
	virtual int filterType() = 0;
protected:
	inline void send(const Message &message);
private:
	Q_DISABLE_COPY(MessageFilter)
	MessageSession *m_session;
};

class JREEN_EXPORT MessageSession : public QObject
{
	Q_OBJECT
	friend class MessageFilter;
public:
	MessageSession(MessageSessionManager *manager, const JID &jid, bool ignore_thread = true, const QString &thread = QString());
	virtual ~MessageSession();
	inline const QString &thread() { return m_thread; }
	inline const JID &jid() { return m_jid; }
	inline bool ignoreThread() { return m_ignore_thread; }
	void resetResource();
	inline void registerMessageFilter(MessageFilter *filter) { if(filter) m_filters.insert(filter->filterType(), filter); }
	template< typename T >
	inline T messageFilter() { return static_cast<T>(m_filters.value(static_cast<T>(0).meta().type, 0)); }
	void sendMessage(const QString &body, const QString &subject = QString());
	virtual void handleMessage(const Message &message);
signals:
	void newMessage(const Message &message);
	void jidChanged(const JID &from, const JID &to);
protected:
	virtual void send(const Message &message);
	inline void filter(Message &message);
	inline void decorate(Message &message);
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

class JREEN_EXPORT MessageSessionManager : public QObject
{
	Q_OBJECT
public:
	MessageSessionManager(Client *client);
	virtual ~MessageSessionManager() {}
	void send(const Message &message);
	void registerMessageSession(MessageSession *session);
	void registerMessageSessionHandler(MessageSessionHandler *handler, QList<Message::Type> types);
	void removeMessageSessionHandler(MessageSessionHandler *handler);
	MessageSession *session(const JID &jid, Message::Type type, bool create = true);
public slots:
	virtual void handleMessage(const jreen::Message &message);
signals:
	void newMessage(const jreen::Message &message);
private:
	Client *m_client;
	QMultiHash<QString, QPointer<MessageSession> > m_full_sessions;
	QMultiHash<QString, QPointer<MessageSession> > m_bare_sessions;
	QVector<MessageSessionHandler *> m_session_handlers;
	friend class MessageSession;
};

inline MessageFilter::MessageFilter(MessageSession *session) : m_session(session)
{
	if(m_session)
		m_session->registerMessageFilter(this);
}

inline void MessageFilter::send(const Message &message)
{
	if(m_session)
		m_session->send(message);
}

inline void MessageSession::filter(Message &message)
{
	foreach(MessageFilter *filter, m_filters)
		filter->filter(message);
}

inline void MessageSession::decorate(Message &message)
{
	foreach(MessageFilter *filter, m_filters)
		filter->decorate(message);
}

}

#endif // MESSAGESESSION_H
