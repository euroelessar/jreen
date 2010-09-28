/****************************************************************************
 *  stanzaextension.h
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

#ifndef STANZAEXTENSION_H
#define STANZAEXTENSION_H

#include <QDomElement>
#include <QObject>
#include <QSharedPointer>
#include "jreen.h"

template <typename T, typename X> class QMap;

namespace jreen
{

struct JREEN_EXPORT StanzaExtensionMeta
{
	StanzaExtensionMeta( const char *name = 0, const char *xpath = 0 );
	const QByteArray name;
	const QString xpath;
	const int type;
};

class JREEN_EXPORT StanzaExtension
{
	Q_DISABLE_COPY(StanzaExtension)
public:
	inline StanzaExtension() {}
	virtual ~StanzaExtension() {}
	virtual QString xPath() const = 0;
	virtual StanzaExtension *fromNode( const QDomElement &node ) const = 0;
	virtual QDomElement node( QDomDocument *document ) const = 0;
	virtual int extensionType() const = 0;
	virtual QByteArray extensionName() const = 0;
};

typedef QSharedPointer<StanzaExtension>   StanzaExtensionPointer;
typedef QMap<int, StanzaExtensionPointer> StanzaExtensionList;

}

#endif // STANZAEXTENSION_H
