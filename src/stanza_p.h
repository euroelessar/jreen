/****************************************************************************
**
** Jreen
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $JREEN_BEGIN_LICENSE$
** Jreen is free software: you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** Jreen is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with Jreen.  If not, see <http://www.gnu.org/licenses/>.
** $JREEN_END_LICENSE$
**
****************************************************************************/

#ifndef JREEN_STANZA_P_H
#define JREEN_STANZA_P_H

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
	struct Token
	{
		enum Type {
			StartElement,
			EndElement,
			Characters
		};
		Token(Type t) : type(t) {}

		Type type;
	};
	
	struct StartToken : public Token
	{
		StartToken() : Token(StartElement) {}

		QStringRef name;
		QStringRef uri;
		QXmlStreamAttributes attributes;
	};
	
	struct EndToken : public Token
	{
		EndToken() : Token(EndElement) {}
	};
	
	struct CharactersToken : public Token
	{
		CharactersToken() : Token(Characters) {}

		QStringRef text;
	};
	
	StanzaPrivate(Type t) : type(t)
	{
		ref = 1;
	}
	virtual ~StanzaPrivate()
	{
		qDeleteAll(tokens);
	}

	void addExtensions(QXmlStreamWriter *writer) const
	{
		Q_UNUSED(writer);
		//foreach(const PayloadPointer &stanzaExtension, extensions)
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
	PayloadList extensions;
	QList<Token*> tokens;
	QString buffer;
};

}

#endif // JREEN_STANZA_P_H
