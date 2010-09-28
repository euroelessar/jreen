/****************************************************************************
 *  presence.cpp
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

#include "presence.h"
#include "stanza_p.h"
#include "langmap.h"
#include "capabilities.h"

J_BEGIN_NAMESPACE

J_STRING(show)
J_STRING(status)
J_STRING(presence)
J_STRING(unavailable)
J_STRING(error)
J_STRING(probe)
J_STRING(away)
J_STRING(chat)
J_STRING(dnd)
J_STRING(xa)
J_STRING(priority)

struct PresencePrivate : public StanzaPrivate
{
	Presence::Type subtype;
	LangMap status;
	int priority;
};

Presence::Presence( Type type, const JID& to, const QString &status, int priority, const QString &xmllang )
	: Stanza(new PresencePrivate)
{
	J_D(Presence);
	j->subtype = type;
	j->to = to;
	j->priority = priority;
	j->status[xmllang] = status;
}

Presence::Presence( const QDomElement &node ) : Stanza(node, new PresencePrivate)
{
	J_D(Presence);
	j->priority = 0;
	if( node.nodeName() != presence_str )
	{
		j->subtype = Invalid;
		return;
	}
	j->subtype = Available;
	QString type = node.attribute( ConstString::type );
	if( type == unavailable_str )
		j->subtype = Unavailable;
	else if( type == error_str )
		j->subtype = Error;
	else if( type == probe_str )
		j->subtype = Probe;
	forelements( const QDomElement &elem, node )
	{
		QString name = elem.nodeName();
		if( j->subtype == Available && name == show_str )
		{
			QString text = elem.text();
			if( text == away_str )
				j->subtype = Away;
			else if( text == chat_str )
				j->subtype = Chat;
			else if( text == dnd_str )
				j->subtype = DND;
			else if( text == xa_str )
				j->subtype = XA;
		}
		else if( name == status_str )
		{
			QString lang = elem.attribute( ConstString::lang );
			j->status[lang] = elem.text();
		}
		else if( name == priority_str )
		{
			j->priority = elem.text().toInt();
		}
	}
}

Presence::Type Presence::subtype() const
{
	J_D(const Presence);
	return j->subtype;
}

const Capabilities *Presence::capabilities() const
{
	return findExtension<Capabilities>().data();
}

Presence::Type Presence::presence() const
{
	J_D(const Presence);
	return j->subtype;
}

void Presence::setPresence( Type type )
{
	J_D(Presence);
	j->subtype = type;
}

const QString &Presence::status( const QString &lang ) const
{
	J_D(const Presence);
	return j->status.value( lang );
}

void Presence::addStatus( const QString &status, const QString &lang )
{
	J_D(Presence);
	j->status[lang] = status;
}

void Presence::resetStatus()
{
	J_D(Presence);
	j->status.clear();
	j->status.clear();
}

int Presence::priority() const
{
	J_D(const Presence);
	return j->priority;
}

void Presence::setPriority( int priority )
{
	J_D(Presence);
	j->priority = priority;
}

QDomElement Presence::node() const
{
	J_D(const Presence);
	if( !j->node.isNull() )
		return j->node;
	QDomElement node = DomCreater::instance().createElement( presence_str );
	j->setAttributes( node );
	switch( j->subtype )
	{
	case Available:
		break;
	case Chat:
		node.appendChild( DomCreater::instance().createElement( show_str, chat_str ));
		break;
	case Away:
		node.appendChild( DomCreater::instance().createElement( show_str, away_str ));
		break;
	case DND:
		node.appendChild( DomCreater::instance().createElement( show_str, dnd_str ));
		break;
	case XA:
		node.appendChild( DomCreater::instance().createElement( show_str, xa_str ));
		break;
	case Unavailable:
		node.setAttribute( ConstString::type, unavailable_str );
		break;
	default:
		return node;
	}
	node.appendChild( DomCreater::instance().createElement( priority_str, QString::number(j->priority) ) );
	j->status.fillNode( node, status_str );
	j->addExtensions( node );
	return node;
}


J_END_NAMESPACE
