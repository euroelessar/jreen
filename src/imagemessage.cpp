/****************************************************************************
 *  imagemessage.cpp
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

#include "imagemessage.h"
#include "jstrings.h"

J_BEGIN_NAMESPACE

J_STRING(img)

ImageMessage::ImageMessage( const QDomElement &node )
{
	if( node.isNull() )
		return;
	m_data = QByteArray::fromBase64( node.text().toLatin1() );
}

ImageMessage::ImageMessage( const QByteArray &data ) : m_data(data)
{
}

QDomElement ImageMessage::node( QDomDocument *doc ) const
{
	if( m_data.isEmpty() )
		return QDomElement();
	QDomElement node = createElement( doc, img_str, m_data.toBase64() );
	node.setAttribute( ConstString::xmlns, ConstString::xmlns_qutim_img );
	return node;
}

J_END_NAMESPACE
