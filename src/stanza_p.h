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
#include <QXmlStreamWriter>
#include "jstrings.h"
#include "stanza.h"
#include "jid.h"

/*
 * WARNING!
 * This file is not a part of JReen API, it may be chagned or even removed
 * without any notification.
 */

namespace Jreen
{
class StanzaPrivate
{
public:
	enum Type {
		StanzaIq,
		StanzaPresence,
		StanzaMessage,
		StanzaSubscription
	};
	
	StanzaPrivate(Type t) : type(t)
	{
		ref = 1;
	}
	void addExtensions(QXmlStreamWriter *writer) const
	{
		Q_UNUSED(writer);
		//foreach(const StanzaExtensionPointer &stanzaExtension, extensions)
		//	stanzaExtension->writeXml(writer);
	}
	void setAttributes(QXmlStreamWriter *writer) const
	{
		if(from.isValid())
			writer->writeAttribute(QLatin1String("from"), from);
		if(to.isValid())
			writer->writeAttribute(QLatin1String("to"), to);
		if(!id.isEmpty())
			writer->writeAttribute(QLatin1String("id"), id);
		writer->writeDefaultNamespace(QLatin1String("jabber:client"));
	}
	static StanzaPrivate *get(Stanza &stanza) { return stanza.d_func(); }
	static const StanzaPrivate *get(const Stanza &stanza) { return stanza.d_func(); }
	Type type;
	QAtomicInt ref;
	JID from;
	JID to;
	QString id;
	StanzaExtensionList extensions;
};

}

#endif // STANZA_P_H
