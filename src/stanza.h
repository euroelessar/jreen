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

class QXmlStreamWriter;

namespace Jreen
{

class StanzaPrivate;
class Error;

class JREEN_EXPORT Stanza
{
	Q_DECLARE_PRIVATE(Stanza)
public:
	typedef QSharedPointer<Stanza> Ptr;
	Stanza(const Stanza &stanza);
	virtual ~Stanza();
	void setFrom(const JID &jid);
	const JID &from() const;
	const JID &to() const;
	const QString &id() const;
	void addExtension(StanzaExtension::Ptr se);
	inline void addExtension(StanzaExtension* se)
	{ addExtension(StanzaExtension::Ptr(se)); }
	const StanzaExtensionList &extensions() const;
	template< class T >
	inline const QSharedPointer<T> findExtension() const
	{ return qSharedPointerCast<T>(extensions().value(reinterpret_cast<T*>(0)->staticExtensionType())); }
	template< class T >
	inline bool containsExtension() const
	{ return extensions().contains(reinterpret_cast<T*>(0)->staticExtensionType()); }
	void removeExtensions();
	const Error *error() const;
	Q_DECL_DEPRECATED virtual void writeXml(QXmlStreamWriter *) const {};
protected:
	Stanza(StanzaPrivate &);
	Stanza &operator =(const Stanza &stanza);
	StanzaPrivate *d_ptr;
};

}

#endif // STANZA_H
