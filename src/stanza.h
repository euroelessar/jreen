/****************************************************************************
 *  stanza.h
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

#ifndef STANZA_H
#define STANZA_H

#include "stanzaextension.h"
#include "jid.h"
#include <QMap>

namespace jreen
{

struct StanzaPrivate;

class JREEN_EXPORT Stanza
{
	Q_DECLARE_PRIVATE(Stanza)
public:
	Stanza( const Stanza &stanza );
	~Stanza();
	void setFrom( const JID &jid );
	const JID &from() const;
	const JID &to() const;
	const QString &id() const;
	void addExtension( StanzaExtension *se );
	const StanzaExtensionList &extensions() const;
	template< class T >
	inline const QSharedPointer<T> findExtension() const
		{ return qSharedPointerCast<T>( extensions().value( static_cast<T *>(0)->meta().type ) ); }
	template< class T >
	inline bool containsExtension() const
		{ return extensions().contains( static_cast<T *>(0)->meta().type ); }
	void removeExtensions();
	virtual QDomElement node() const = 0;
	inline operator QDomElement() const { return node(); }
protected:
	Stanza( const JID &to );
	Stanza( const QDomElement &node, StanzaPrivate *sp = 0 );
	Stanza( StanzaPrivate * );
	Stanza &operator =( const Stanza &stanza );
	StanzaPrivate *d_ptr;
};

}

#endif // STANZA_H
