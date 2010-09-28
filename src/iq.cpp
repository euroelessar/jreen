/****************************************************************************
 *  iq.cpp
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

#include "iq.h"
#include "stanza_p.h"
#include <QStringList>

J_BEGIN_NAMESPACE

J_STRING(iq)

static const QStringList iq_types = QStringList() << QLatin1String("get") << QLatin1String("set")
									<< QLatin1String("result") << QLatin1String("error");

struct IQPrivate : public StanzaPrivate
{
	IQPrivate() : accepted(false) {}
	IQ::Type subtype;
	mutable bool accepted;
};

IQ::IQ( Type type, const JID& to, const QString& id ) : Stanza(new IQPrivate)
{
	J_D(IQ);
	j->subtype = type;
	j->to = to;
	j->id = id;
}

IQ::IQ( const QDomElement &node ) : Stanza(node, new IQPrivate)
{
	J_D(IQ);
	if( node.nodeName() != iq_str )
	{
		j->subtype = Invalid;
		return;
	}
	int type = iq_types.indexOf( node.attribute( ConstString::type ) );
	j->subtype = type < 0 ? Invalid : static_cast<Type>( type );
}

IQ::Type IQ::subtype() const
{
	J_D(const IQ);
	return j->subtype;
}

QDomElement IQ::node() const
{
	J_D(const IQ);
	if( !j->node.isNull() )
		return j->node;
	QDomElement node = DomCreater::instance().createElement( iq_str );
	j->setAttributes( node );
	if( j->subtype == Invalid )
		return node;
	node.setAttribute( ConstString::type, iq_types.at( j->subtype ) );
	j->addExtensions( node );
	return node;
}

void IQ::accept() const
{
	J_D(const IQ);
	j->accepted = true;
}

bool IQ::accepted() const
{
	J_D(const IQ);
	return j->accepted;
}

J_END_NAMESPACE
