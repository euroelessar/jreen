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

namespace jreen
{

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
	Q_D(Message);
	d->subtype = type;
	d->to = to;
	d->thread = thread;
	d->body[xmllang] = body;
	d->subject[xmllang] = subject;
}

Message::Message( const QDomElement &node ) : Stanza(node, new MessagePrivate)
{
	Q_D(Message);
	d->subtype = Invalid;
	if( node.nodeName() != message_str )
		return;
	QString type_str = node.attribute( ConstString::type );
	if( type_str.isEmpty() )
		d->subtype = Normal;
	else
	{
		int type = message_types.indexOf( type_str );
		d->subtype = type < 0 ? Invalid : static_cast<Type>( type );
	}
	forelements( const QDomElement &elem, node )
	{
		QString name = elem.nodeName();
		if( name == body_str )
		{
			QString lang = elem.attribute( ConstString::lang );
			d->body[lang] = elem.text();
		}
		else if( name == subject_str )
		{
			QString lang = elem.attribute( ConstString::lang );
			d->subject[lang] = elem.text();
		}
		else if( name == thread_str )
			d->thread = elem.text();
	}
}

Message::Type Message::subtype() const
{
	Q_D(const Message);
	return d->subtype;
}

const QString &Message::body( const QString &lang ) const
{
	Q_D(const Message);
	return d->body.value( lang );
}

const QString &Message::subject( const QString &lang ) const
{
	Q_D(const Message);
	return d->subject.value( lang );
}

const QString &Message::thread() const
{
	Q_D(const Message);
	return d->thread;
}

void Message::setThread( const QString &thread )
{
	Q_D(Message);
	d->thread = thread;
}

void Message::setID( const QString &id )
{
	Q_D(Message);
	d->id = id;
}

const DelayedDelivery *Message::when() const
{
	return findExtension<DelayedDelivery>().data();
}

QDomElement Message::node() const
{
	Q_D(const Message);
	if( !d->node.isNull() )
		return d->node;
	QDomElement node = DomCreater::instance().createElement( message_str );
	d->setAttributes( node );
	if( d->subtype == Invalid )
		return node;
	d->subject.fillNode( node, subject_str );
	d->body.fillNode( node, body_str );
	if( !d->thread.isEmpty() )
		createElement( node, thread_str, d->thread );
	node.setAttribute( ConstString::type, message_types.at( d->subtype ) );
	d->addExtensions( node );
	return node;
}


}
