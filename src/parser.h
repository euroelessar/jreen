/*****************************************************************************
 *  parser.h
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#include "stanza.h"
#include <QXmlStreamReader>
#include <QEvent>

namespace Jreen
{
class Client;
class ParserPrivate;

class Parser : public QObject, public XmlStreamParser
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(Parser)
public:
	enum State
	{
		WaitingForStanza,
		ReadFeatures,
		ReadStanza,
		ReadCustom
	};

	Parser(Client *client);
	~Parser();
	void reset();
	void activateFeature();
	
	bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleEndElement(const QStringRef &name, const QStringRef &uri);
	void handleCharacterData(const QStringRef &text);
	
	bool event(QEvent *ev);
	void appendData(const QByteArray &a);
	void parseData();
private:
	QScopedPointer<ParserPrivate> d_ptr;
};
}

#endif
