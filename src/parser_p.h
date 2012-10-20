/****************************************************************************
**
** Jreen
**
** Copyright © 2003 Justin Karneges <justin@affinix.com>
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $JREEN_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $JREEN_END_LICENSE$
**
****************************************************************************/

#ifndef PARSER_P_H
#define PARSER_P_H

#include "jreen.h"
#include "parser.h"
#include "streamfeature_p.h"
#include "stanzaextension.h"
#include "stanza.h"
#include "stanzafactory_p.h"
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
	QXmlStreamReader nullReader;
	Parser::State state;
	QStack<XmlStreamParser*> parsers;
	QStack<int> parsersCount;
	QByteArray buffer;
	int depth;
	bool atParsing;
	bool first;
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
