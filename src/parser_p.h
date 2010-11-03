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

/*
 * WARNING!
 * This file is not a part of JReen API, it may be chagned or even removed
 * without any notification.
 */

namespace jreen
{
	class ParserPrivate
	{
	public:
		QXmlStreamReader *reader;
		Parser::State state;
		QStack<XmlStreamParser*> parsers;
		QStack<int> parsersCount;
		Stanza::Ptr stanza;
		int depth;
		ClientPrivate *client;
		
		inline bool canParse(XmlStreamParser *parser)
		{ return parser->canParse(reader->name(), reader->namespaceUri(), reader->attributes()); }
	};
}

#endif // PARSER_P_H
