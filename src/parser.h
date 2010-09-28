/*****************************************************************************
 *  parser.h
 *
 *  Copyright (c) 2003 by Justin Karneges
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#ifndef PARSER_H
#define PARSER_H

#include <QDomElement>
#include <QXmlInputSource>
#include "jreen.h"

namespace jreen
{

class ParserPrivate;

class Parser : public QObject
{
	QScopedPointer<ParserPrivate> d_ptr;
	Q_OBJECT
public:
	class EventPrivate;
	class Event
	{
		friend class Parser;
		Q_DECLARE_PRIVATE(Event)
		EventPrivate *d_ptr;
	public:
		enum Type { DocumentOpen = 0, DocumentClose = 1, Element = 2, Error = 3 };

		Event();
		Event(const Event &e);
		Event &operator=(const Event &e);
		~Event();

		Type type();

		// for document open
		const QString &nsprefix(const QString &s = QString()) const;

		// for document open / close
		const QString &namespaceURI() const;
		const QString &localName() const;
		const QString &qName() const;
		const QXmlAttributes &atts() const;

		// for element
		const QDomElement &element() const;

		// for any
		const QString actualString() const;

		// setup
		void setDocumentOpen(const QString &namespaceURI, const QString &localName, const QString &qName,
							  const QXmlAttributes &atts, const QStringList &nsnames, const QStringList &nsvalues);
		void setDocumentClose(const QString &namespaceURI, const QString &localName, const QString &qName);
		void setElement(const QDomElement &elem);
		void setError();
		void setActualString(const QString &str);

		bool isNull() const;
		inline operator bool() const { return !isNull(); }
	};
	Parser();
	~Parser();
	void reset();
	void appendData(const QByteArray &a);
	Event readNext();
	QByteArray unprocessed() const;
	QString encoding() const;
};

}

#endif
