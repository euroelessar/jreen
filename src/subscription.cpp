/****************************************************************************
 *  subscription.cpp
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

#include "subscription.h"
#include "stanza_p.h"
#include "langmap.h"

J_BEGIN_NAMESPACE

J_STRING(presence)
J_STRING(status)

static const QStringList s10n_types = QStringList() << QLatin1String("subscribe") << QLatin1String("subscribed")
									  << QLatin1String("unsubscribe") << QLatin1String("unsubscribed");

struct SubscriptionPrivate : public StanzaPrivate
{
	Subscription::Type subtype;
	LangMap status;
};

Subscription::Subscription( const QDomElement &node ) : Stanza(node, new SubscriptionPrivate)
{
	J_D(Subscription);
	int type = s10n_types.indexOf( node.attribute( ConstString::type ) );
	j->subtype = type < 0 ? Invalid : static_cast<Type>( type );
	forelements( const QDomElement &elem, node )
		if(  elem.nodeName() == status_str )
		{
			QString lang = elem.attribute( ConstString::lang );
			j->status[lang] = elem.text();
		}
}

Subscription::Subscription( Type type, const JID& to, const QString &status, const QString &xmllang ) : Stanza(new SubscriptionPrivate)
{
	J_D(Subscription);
	j->subtype = type;
	j->to = to;
	j->status[xmllang] = status;
}

Subscription::Type Subscription::subtype() const
{
	J_D(const Subscription);
	return j->subtype;
}

const QString &Subscription::status( const QString &lang ) const
{
	J_D(const Subscription);
	return j->status.value( lang );
}

QDomElement Subscription::node() const
{
	J_D(const Subscription);
	if( !j->node.isNull() )
		return j->node;
	QDomElement node = DomCreater::instance().createElement( presence_str );
	j->setAttributes( node );
	if( j->subtype == Invalid )
		return node;
	j->status.fillNode( node, status_str );
	node.setAttribute( ConstString::type, s10n_types.at( j->subtype ) );
	j->addExtensions( node );
	return node;
}

J_END_NAMESPACE
