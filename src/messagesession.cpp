/****************************************************************************
 *  messagesession.cpp
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

#include "messagesession.h"
#include "client.h"
#include "util.h"
#include <QDebug>

namespace jreen
{

J_STRING(MessageSession)

static int message_filter_count = 0;

MessageFilterMeta::MessageFilterMeta() : type(message_filter_count++)
{
}

MessageSession::MessageSession( MessageSessionManager *manager, const JID &jid, bool ignore_thread, const QString &thread )
		: QObject(manager), m_manager(manager), m_ignore_thread(ignore_thread), m_want_upgrade(false), m_jid(jid)
{
	setObjectName(MessageSession_str);
	if( manager )
		manager->registerMessageSession( this );
	m_thread = thread;
}

MessageSession::~MessageSession()
{
	qDeleteAll( m_filters );
	m_manager->m_full_sessions.remove( m_jid.full(), this );
	m_manager->m_bare_sessions.remove( m_jid.bare(), this );
}

void MessageSession::resetResource()
{
	m_want_upgrade = true;
	setJid( m_jid.bareJID() );
	foreach( MessageFilter *filter, m_filters )
		filter->reset();
}

void MessageSession::sendMessage( const QString &body, const QString &subject )
{
	if( !m_manager )
		return;
	Message message( Message::Chat, m_jid, body, subject, m_thread );
	decorate( message );
	m_manager->send( message );
}

void MessageSession::handleMessage( const Message &message_orig )
{
	Message message = message_orig;
	if( m_want_upgrade && message.from().bare() == m_jid.full() )
		setJid( message.from() );
	if( m_thread.isEmpty() )
	{
		if( message.thread().trimmed().isEmpty() )
			message.setThread( m_thread = Util::randomHash( message.from() ) );
		else
			m_thread = message.thread();
	}
	filter( message );
	if( !message.body().isEmpty() )
		emit newMessage( message );
}

void MessageSession::send( const Message &message )
{
	if( m_manager )
		m_manager->send( message );
}

void MessageSession::setJid( const JID &jid )
{
	if( m_jid != jid )
	{
		emit jidChanged( m_jid, jid );
		if( m_manager )
		{
			m_manager->m_full_sessions.remove( m_jid, this );
			m_manager->m_bare_sessions.remove( m_jid.bare(), this );
			m_manager->m_full_sessions.insert( jid.full(), this );
			m_manager->m_bare_sessions.insert( jid.bare(), this );
		}
		m_jid = jid;
	}
}

MessageSessionManager::MessageSessionManager( Client *client ) : m_client(client), m_session_handlers(Message::Invalid)
{
	qsrand( QDateTime::currentDateTime().toTime_t() );
	MessageSessionManager::connect( client, SIGNAL(newMessage(Message)), this, SLOT(handleMessage(Message)) );
}

void MessageSessionManager::send( const Message &message )
{
	m_client->send( message );
}

void MessageSessionManager::registerMessageSession( MessageSession *session )
{
	if( !session || !session->jid().isValid() )
		return;
	m_bare_sessions.insert( session->jid().bare(), session );
	m_full_sessions.insert( session->jid(), session );
}

void MessageSessionManager::registerMessageSessionHandler( MessageSessionHandler *handler, QList<Message::Type> types )
{
	for( int i = 0; i < types.size(); i++ )
		m_session_handlers[i] = handler;
}

void MessageSessionManager::removeMessageSessionHandler( MessageSessionHandler *handler )
{
	for( int i = 0; i < m_session_handlers.size(); i++ )
		if( m_session_handlers[i] == handler )
			m_session_handlers[i] = 0;
}

MessageSession *MessageSessionManager::session( const JID &jid, Message::Type type, bool create )
{
	QList<QPointer<MessageSession> > sessions = m_full_sessions.values( jid.full() );
	qDebug() << "m_full_sessions" << m_full_sessions;
	foreach( MessageSession *session, sessions )
		qDebug() << "MessageSession" << (session ? session->jid() : JID());
	for( int i = 0; i < sessions.size(); i++ )
	{
		if( sessions[i].isNull() )
		{
			m_full_sessions.remove( jid.full(), sessions[i] );
			continue;
		}
		return sessions[i];
	}
	if( !create )
		return 0;
	MessageSessionHandler *handler = m_session_handlers.value( type );
	if( !handler )
		return 0;
	MessageSession *session = new MessageSession( this, jid.full(), false, QString() );
	handler->handleMessageSession( session );
	return session;
}

void MessageSessionManager::handleMessage( const Message &message )
{
	QList<QPointer<MessageSession> > sessions = m_full_sessions.values( message.from() );
	for( int i = 0; i < sessions.size(); i++ )
	{
		if( sessions[i].isNull() )
		{
			m_full_sessions.remove( message.from(), sessions[i] );
			continue;
		}
		if( message.thread().isEmpty() || sessions[i]->ignoreThread() || sessions[i]->thread() == message.thread() )
		{
			sessions[i]->handleMessage( message );
			return;
		}
	}
	sessions = m_bare_sessions.values( message.from().bare() );
	for( int i = 0; i < sessions.size(); i++ )
	{
		if( sessions[i].isNull() )
		{
			m_bare_sessions.remove( message.from(), sessions[i] );
			continue;
		}
		if( message.thread().isEmpty() || sessions[i]->ignoreThread() || sessions[i]->thread() == message.thread() )
		{
			sessions[i]->handleMessage( message );
			return;
		}
	}

	MessageSessionHandler *handler = m_session_handlers.value( message.subtype() );
	if( !handler )
	{
		emit newMessage( message );
		return;
	}
	MessageSession *session = new MessageSession( this, message.from(), false, message.thread() );
	handler->handleMessageSession( session );
	session->handleMessage( message );
}

}
