/****************************************************************************
 *  receipt.cpp
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

#include "receipt.h"
#include "jstrings.h"
#include <QStringList>

J_BEGIN_NAMESPACE

static const QStringList receipt_types = QStringList()
										 << QLatin1String("request") << QLatin1String("received");

Receipt::Receipt( const QDomElement &node )
{
	if( node.isNull() )
		m_type = Invalid;
	else
	{
		int type = receipt_types.indexOf( node.nodeName() );
		m_type = type < 0 ? Invalid : static_cast<Type>( type );
	}
}

Receipt::Receipt( Type type ) : m_type(type)
{
}

QDomElement Receipt::node( QDomDocument *doc ) const
{
	if( m_type == Invalid )
		return QDomElement();
	QDomElement node = createElement( doc, receipt_types.at( m_type ) );
	node.setAttribute( ConstString::xmlns, ConstString::xmlns_receipts );
	return node;
}

J_END_NAMESPACE
