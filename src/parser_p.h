/*****************************************************************************
 *  parser_p.h
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

#ifndef PARSER_P_H
#define PARSER_P_H

#include "jreen.h"
#include "parser.h"
#include "streamfeature.h"
#include "stanzaextension.h"
#include "stanza.h"
#include "stanzafactory.h"
#include "client_p.h"
#include <QStack>

// #define PARSER_DEBUG_SPEED 1
#undef PARSER_DEBUG_SPEED
#define PARSER_SPLIT_STANZAS_EVENTS 1

/*
 * WARNING!
 * This file is not a part of JReen API, it may be chagned or even removed
 * without any notification.
 */

namespace Jreen
{
class ParserPrivate
{
public:
	QXmlStreamReader *reader;
	Parser::State state;
	QStack<XmlStreamParser*> parsers;
	QStack<int> parsersCount;
	QList<StanzaExtension::Ptr> extensions;
	int depth;
	bool atParsing;
	ClientPrivate *client;
#ifdef PARSER_DEBUG_SPEED
	int parsingTime;
	int totalParsingTime;
	int totalLogicTime;
	int stanzaLogicTime[4];
#endif
};
}

#endif // PARSER_P_H
