/****************************************************************************
 *  stanza_p.h
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

#ifndef STANZA_P_H
#define STANZA_P_H

#include <QHash>
#include <QMap>
#include "jstrings.h"
#include "stanza.h"
#include "jid.h"
#include "domcreater_p.h"

/*
 * WARNING!
 * This file is not a part of JReen API, it may be chagned or even removed
 * without any notification.
 */

namespace jreen
{

//#if (QT_VERSION >= QT_VERSION_CHECK(4, 5, 0))
//typedef QSharedPointer<StanzaExtension> StanzaExtensionPointer;
//#else
//struct StanzaExtensionPointerPrivate
//{
//	StanzaExtensionPointerPrivate( StanzaExtension *ext )
//	{
//		ref = 1;
//		ptr = ext;
//	}
//	QAtomicInt ref;
//	StanzaExtension *ptr;
//};
//class StanzaExtensionPointer
//{
//	StanzaExtensionPointerPrivate *impl;
//public:
//	inline StanzaExtensionPointer( StanzaExtension *ext ) : impl(new StanzaExtensionPointerPrivate(ext)) {}
//	StanzaExtensionPointer( const StanzaExtensionPointer &pointer )
//	{
//		pointer.impl->ref.ref();
//		if( !impl->ref.deref() )
//			delete impl;
//		impl = pointer.impl;
//	}
//	~StanzaExtensionPointer()
//	{
//		if( !impl->ref.deref() )
//			delete impl->ptr;
//	}
//	StanzaExtensionPointer &operator =( const StanzaExtensionPointer &pointer )
//	{
//		pointer.impl->ref.ref();
//		if( !impl->ref.deref() )
//			delete impl;
//		impl = pointer.impl;
//		return *this;
//	}
//	inline StanzaExtension *data() const { return impl->ptr; }
//	inline StanzaExtension *operator ->() const { return impl->ptr; }
//	inline StanzaExtension &operator *() const { return *impl->ptr; }
//};
//#endif

struct StanzaPrivate
{
	StanzaPrivate()
	{
		ref = 1;
	}
	void addExtensions( QDomElement &node ) const
	{
		foreach( const StanzaExtensionPointer &stanza_extension, extensions )
		{
			node.appendChild( stanza_extension->node( DomCreater::instance().document() ) );
		}
	}
	void setAttributes( QDomElement &node ) const
	{
		if( from.isValid() )
			node.setAttribute( ConstString::from, from );
		if( to.isValid() )
			node.setAttribute( ConstString::to, to );
		if( !id.isEmpty() )
			node.setAttribute( ConstString::id, id );
	}
	QAtomicInt ref;
	JID from;
	JID to;
	QString id;
	QDomElement node;
	StanzaExtensionList extensions;
};

}

#endif // STANZA_P_H
