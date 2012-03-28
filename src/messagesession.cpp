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

#include "messagesession.h"
#include "client_p.h"
#include "util.h"
#include "logger.h"

namespace Jreen
{

static int message_filter_count = 0;

class MessageSessionManagerPrivate
{
public:
	Client *client;
	QMultiHash<QString, QPointer<MessageSession> > fullSessions;
	QMultiHash<QString, QPointer<MessageSession> > bareSessions;
	QVector<MessageSessionHandler *> sessionHandlers;
};

MessageFilter::MessageFilter(MessageSession *session) : m_session(session)
{

}

void MessageFilter::send(const Message &message)
{
	if(m_session)
		m_session->send(message);
}

void MessageSession::filter(Message &message)
{
	foreach(MessageFilter *filter, m_filters)
		filter->filter(message);
}

void MessageSession::decorate(Message &message)
{
	foreach(MessageFilter *filter, m_filters)
		filter->decorate(message);
}


MessageFilterMeta::MessageFilterMeta() : type(message_filter_count++)
{
}

MessageSession::MessageSession(MessageSessionManager *manager, const JID &jid, bool ignore_thread, const QString &thread)
		: QObject(manager), m_manager(manager), m_ignore_thread(ignore_thread), m_want_upgrade(false), m_jid(jid)
{
	setObjectName(QLatin1String("MessageSession"));
	if(manager)
		manager->registerMessageSession(this);
	m_thread = thread;
}

MessageSession::~MessageSession()
{
	qDeleteAll(m_filters);
	m_manager->d_func()->fullSessions.remove(m_jid.full(), this);
	m_manager->d_func()->bareSessions.remove(m_jid.bare(), this);
}

void MessageSession::resetResource()
{
	m_want_upgrade = true;
	setJid(m_jid.bareJID());
	foreach(MessageFilter *filter, m_filters)
		filter->reset();
}

void MessageSession::sendMessage(const QString &body, const QString &subject)
{
	Message message(Message::Chat, m_jid, body, subject);
	sendMessage(message);
}

void MessageSession::sendMessage(const Message &message)
{
	if(!m_manager)
		return;
	Message msg = message;
	msg.setThread(m_thread);
	decorate(msg);
	m_manager->send(msg);
}

void MessageSession::handleMessage(const Message &message_orig)
{
	Message message = message_orig;
	if(m_want_upgrade && message.from().bare() == m_jid.full())
		setJid(message.from());
	if(m_thread.isEmpty())
	{
		if(message.thread().trimmed().isEmpty())
			message.setThread(m_thread = QLatin1String(Util::randomHash()));
		else
			m_thread = message.thread();
	}
	filter(message);
	if(!message.body().isEmpty())
		emit messageReceived(message);
}

void MessageSession::send(const Message &message)
{
	if(m_manager)
		m_manager->send(message);
}

void MessageSession::setJid(const JID &jid)
{
	if(m_jid != jid)
	{
		emit jidChanged(m_jid, jid);
		if(m_manager)
		{
			MessageSessionManagerPrivate *manager = m_manager->d_func();
			manager->fullSessions.remove(m_jid, this);
			manager->bareSessions.remove(m_jid.bare(), this);
			manager->fullSessions.insert(jid.full(), this);
			manager->bareSessions.insert(jid.bare(), this);
		}
		m_jid = jid;
	}
}

MessageSessionManager::MessageSessionManager(Client *client) :
	d_ptr(new MessageSessionManagerPrivate)
{
	Q_D(MessageSessionManager);
	d->client = client;
	d->sessionHandlers.resize(Message::Invalid + 1);
	qsrand(QDateTime::currentDateTime().toTime_t());
	connect(client, SIGNAL(messageReceived(Jreen::Message)),
	        this, SLOT(handleMessage(Jreen::Message)));
	ClientPrivate::get(d->client)->messageSessionManager = this;
}

MessageSessionManager::~MessageSessionManager()
{

}

void MessageSessionManager::send(const Message &message)
{
	d_func()->client->send(message);
}

void MessageSessionManager::registerMessageSession(MessageSession *session)
{
	Q_D(MessageSessionManager);
	if(!session || !session->jid().isValid())
		return;
	d->bareSessions.insert(session->jid().bare(), session);
	d->fullSessions.insert(session->jid(), session);
}

void MessageSessionManager::registerMessageSessionHandler(MessageSessionHandler *handler, QList<Message::Type> types)
{
	for (int i = 0; i < types.size(); i++) {
		Q_ASSERT(types.at(i) >= 0 && types.at(i) <= Message::Invalid);
		d_func()->sessionHandlers[types.at(i)] = handler;
	}
}

void MessageSessionManager::removeMessageSessionHandler(MessageSessionHandler *handler)
{
	Q_D(MessageSessionManager);
	for (int i = 0; i < d->sessionHandlers.size(); i++) {
		if(d->sessionHandlers[i] == handler)
			d->sessionHandlers[i] = 0;
	}
}

MessageSession *MessageSessionManager::session(const JID &jid, Message::Type type, bool create)
{
	Q_D(MessageSessionManager);
	QList<QPointer<MessageSession> > sessions = d->fullSessions.values(jid.full());
	Logger::debug() << "d->full_sessions" << d->fullSessions;
	foreach(MessageSession *session, sessions)
		Logger::debug() << "MessageSession" << (session ? session->jid() : JID());
	for(int i = 0; i < sessions.size(); i++)
	{
		if(sessions[i].isNull())
		{
			d->fullSessions.remove(jid.full(), sessions[i]);
			continue;
		}
		return sessions[i];
	}
	if(!create)
		return 0;
	MessageSessionHandler *handler = d->sessionHandlers.value(type);
	if(!handler)
		return 0;
	MessageSession *session = new MessageSession(this, jid.full(), false, QString());
	handler->handleMessageSession(session);
	return session;
}

void MessageSessionManager::handleMessage(const Message &message)
{
	Q_D(MessageSessionManager);
	QList<QPointer<MessageSession> > sessions = d->fullSessions.values(message.from());
	for(int i = 0; i < sessions.size(); i++)
	{
		if(sessions[i].isNull())
		{
			d->fullSessions.remove(message.from(), sessions[i]);
			continue;
		}
		if(message.thread().isEmpty() || sessions[i]->ignoreThread() || sessions[i]->thread() == message.thread())
		{
			sessions[i]->handleMessage(message);
			return;
		}
	}
	sessions = d->bareSessions.values(message.from().bare());
	for(int i = 0; i < sessions.size(); i++)
	{
		if(sessions[i].isNull())
		{
			d->bareSessions.remove(message.from(), sessions[i]);
			continue;
		}
		if(message.thread().isEmpty() || sessions[i]->ignoreThread() || sessions[i]->thread() == message.thread())
		{
			sessions[i]->handleMessage(message);
			return;
		}
	}

	MessageSessionHandler *handler = d->sessionHandlers.value(message.subtype());
	if(!handler)
	{
		emit messageReceived(message);
		return;
	}
	MessageSession *session = new MessageSession(this, message.from(), false, message.thread());
	handler->handleMessageSession(session);
	session->handleMessage(message);
}


void MessageSession::registerMessageFilter(MessageFilter *filter)
{
	m_filters.insert(filter->filterType(),filter);
}

}

