/****************************************************************************
 *  softwareversion.cpp
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

#include "softwareversion.h"
#include "jstrings.h"

J_BEGIN_NAMESPACE

J_STRING(name)
J_STRING(version)
J_STRING(os)

SoftwareVersion::SoftwareVersion( const QString &name, const QString &version, const QString &os )
	: m_name(name), m_version(version), m_os(os)
{
}

SoftwareVersion::SoftwareVersion( const QDomElement &node )
{
	if( node.isNull() )
		return;
	forelements( const QDomElement &elem, node )
	{
		if( elem.nodeName() == name_str )
			m_name = elem.text();
		else if( elem.nodeName() == version_str )
			m_version = elem.text();
		else if( elem.nodeName() == os_str )
			m_os = elem.text();
	}
}

QDomElement SoftwareVersion::node( QDomDocument *document ) const
{
	QDomElement node = createElement( document, ConstString::query );
	node.setAttribute( ConstString::xmlns, ConstString::xmlns_version );
	if( !m_name.isEmpty() )
		node.appendChild( createElement( document, name_str, m_name ) );
	if( !m_version.isEmpty() )
		node.appendChild( createElement( document, version_str, m_version ) );
	if( !m_os.isEmpty() )
		node.appendChild( createElement( document, os_str, m_os ) );
	return node;
}


J_END_NAMESPACE
