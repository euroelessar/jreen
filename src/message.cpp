/****************************************************************************
 *  message.cpp
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

#include "message.h"
#include "stanza_p.h"
#include "langmap.h"
#include "delayeddelivery.h"
#include "util.h"
#include <QStringList>

J_BEGIN_NAMESPACE

J_STRING(message)
J_STRING(body)
J_STRING(subject)
J_STRING(thread)

static const QStringList message_types = QStringList()
										 << QLatin1String("chat") << QLatin1String("error")
										 << QLatin1String("groupchat") << QLatin1String("headline")
										 << QLatin1String("normal");

struct MessagePrivate : public StanzaPrivate
{
	Message::Type subtype;
	LangMap body;
	LangMap subject;
	QString thread;
};

Message::Message( Type type, const JID& to, const QString &body, const QString &subject, const QString &thread, const QString &xmllang )
	: Stanza(new MessagePrivate)
{
	J_D(Message);
	j->subtype = type;
	j->to = to;
	j->thread = thread;
	j->body[xmllang] = body;
	j->subject[xmllang] = subject;
}

Message::Message( const QDomElement &node ) : Stanza(node, new MessagePrivate)
{
	J_D(Message);
	j->subtype = Invalid;
	if( node.nodeName() != message_str )
		return;
	QString type_str = node.attribute( ConstString::type );
	if( type_str.isEmpty() )
		j->subtype = Normal;
	else
	{
		int type = message_types.indexOf( type_str );
		j->subtype = type < 0 ? Invalid : static_cast<Type>( type );
	}
	forelements( const QDomElement &elem, node )
	{
		QString name = elem.nodeName();
		if( name == body_str )
		{
			QString lang = elem.attribute( ConstString::lang );
			j->body[lang] = elem.text();
		}
		else if( name == subject_str )
		{
			QString lang = elem.attribute( ConstString::lang );
			j->subject[lang] = elem.text();
		}
		else if( name == thread_str )
			j->thread = elem.text();
	}
}

Message::Type Message::subtype() const
{
	J_D(const Message);
	return j->subtype;
}

const QString &Message::body( const QString &lang ) const
{
	J_D(const Message);
	return j->body.value( lang );
}

const QString &Message::subject( const QString &lang ) const
{
	J_D(const Message);
	return j->subject.value( lang );
}

const QString &Message::thread() const
{
	J_D(const Message);
	return j->thread;
}

void Message::setThread( const QString &thread )
{
	J_D(Message);
	j->thread = thread;
}

void Message::setID( const QString &id )
{
	J_D(Message);
	j->id = id;
}

const DelayedDelivery *Message::when() const
{
	return findExtension<DelayedDelivery>().data();
}

QDomElement Message::node() const
{
	J_D(const Message);
	if( !j->node.isNull() )
		return j->node;
	QDomElement node = DomCreater::instance().createElement( message_str );
	j->setAttributes( node );
	if( j->subtype == Invalid )
		return node;
	j->subject.fillNode( node, subject_str );
	j->body.fillNode( node, body_str );
	if( !j->thread.isEmpty() )
		createElement( node, thread_str, j->thread );
	node.setAttribute( ConstString::type, message_types.at( j->subtype ) );
	j->addExtensions( node );
	return node;
}


J_END_NAMESPACE
