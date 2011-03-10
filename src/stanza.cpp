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
#include "error.h"
#include <QTextStream>

namespace Jreen
{

Stanza::Stanza(const Stanza &stanza)
{
	if(stanza.d_ptr)
		stanza.d_ptr->ref.ref();
	d_ptr = stanza.d_ptr;
}

Stanza &Stanza::operator =(const Stanza &stanza)
{
	if(stanza.d_ptr)
		stanza.d_ptr->ref.ref();
	if(!d_ptr->ref.deref())
		delete d_ptr;
	d_ptr = stanza.d_ptr;
	return *this;
}

Stanza::Stanza(StanzaPrivate &sp)
{
	d_ptr = &sp;
}

Stanza::~Stanza()
{
	if(d_ptr && !d_ptr->ref.deref())
		delete d_ptr;
}

void Stanza::setFrom(const JID &jid)
{
	d_ptr->from = jid;
}

const JID &Stanza::from() const
{
	return d_ptr->from;
}

const JID &Stanza::to() const
{
	return d_ptr->to;
}

const QString &Stanza::id() const
{
	return d_ptr->id;
}

void Stanza::addExtension(StanzaExtension::Ptr se)
{
	d_ptr->extensions.insert(se->extensionType(), se);
}

const StanzaExtensionList &Stanza::extensions() const
{
	return d_ptr->extensions;
}

void Stanza::removeExtensions()
{
	d_ptr->extensions.clear();
}

const Error *Stanza::error() const
{
	return findExtension<Error>().data();
}

}
