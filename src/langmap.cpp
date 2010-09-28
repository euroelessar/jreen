/****************************************************************************
 *  langmap.cpp
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

#include "langmap.h"
#include "jstrings.h"

namespace jreen
{

LangMapData LangMapData::shared_null = { Q_BASIC_ATOMIC_INITIALIZER(1), QString(), LangMapData::Base() };

void LangMapData::fillNode( QDomElement &node, const QString &name, const QString &uri ) const
{
	QDomDocument doc = node.ownerDocument();
	if( node.isNull() || doc.isNull() )
		return;
	if( !base.isEmpty() )
		node.appendChild( createElement( &doc, name, base ) );
	for( LangMapData::Base::const_iterator i = other.begin(); i != other.end(); i++ )
	{
		QDomElement elem = createElement( &doc, name, i.value() );
		elem.setAttribute( ConstString::lang, i.key() );
		if( !uri.isEmpty() )
			elem.setAttribute( ConstString::xmlns, uri );
		node.appendChild( elem );
	}
}

const QString &LangMapData::value(const QString &lang ) const
{
	if( !lang.isEmpty() )
	{
		LangMapData::Base::const_iterator iter = other.find( lang );
		if( iter != other.end() )
			return iter.value();
	}
	return base;
}

const QString &LangMapData::value(const QString &lang, const QString &default_value) const
{
	if( !lang.isEmpty() )
	{
		LangMapData::Base::const_iterator iter = other.find( lang );
		if( iter != other.end() )
			return iter.value();
	}
	return default_value;
}

}
