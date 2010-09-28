/****************************************************************************
 *  stanza.cpp
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

#include "stanza.h"
#include "stanza_p.h"
#include <QTextStream>

J_BEGIN_NAMESPACE

Stanza::Stanza( const Stanza &stanza )
{
	if( stanza.j_ptr )
		stanza.j_ptr->ref.ref();
	j_ptr = stanza.j_ptr;
}

Stanza::Stanza( const JID &to )
{
	j_ptr = new StanzaPrivate;
	j_ptr->to = to;
}

Stanza &Stanza::operator =( const Stanza &stanza )
{
	if( stanza.j_ptr )
		stanza.j_ptr->ref.ref();
	if( !j_ptr->ref.deref() )
		delete j_ptr;
	j_ptr = stanza.j_ptr;
	return *this;
}

Stanza::Stanza( const QDomElement &node, StanzaPrivate *sp )
{
	if( !sp )
		j_ptr = new StanzaPrivate;
	else
		j_ptr = sp;
	j_ptr->node = node;
	j_ptr->from = node.attribute( ConstString::from );
	j_ptr->to = node.attribute( ConstString::to );
	j_ptr->id = node.attribute( ConstString::id );
}

Stanza::Stanza( StanzaPrivate *sp )
{
	j_ptr = sp;
}

Stanza::~Stanza()
{
	if( j_ptr && !j_ptr->ref.deref() )
		delete j_ptr;
}

void Stanza::setFrom( const JID &jid )
{
	j_ptr->from = jid;
}

const JID &Stanza::from() const
{
	return j_ptr->from;
}

const JID &Stanza::to() const
{
	return j_ptr->to;
}

const QString &Stanza::id() const
{
	return j_ptr->id;
}

void Stanza::addExtension( StanzaExtension *se )
{
	j_ptr->extensions.insert( se->extensionType(), StanzaExtensionPointer( se ) );
}

const StanzaExtensionList &Stanza::extensions() const
{
	return j_ptr->extensions;
}

void Stanza::removeExtensions()
{
	j_ptr->extensions.clear();
}


J_END_NAMESPACE
