/*****************************************************************************
 *  parser.cpp
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

#include "parser_p.h"

#include <qtextcodec.h>

J_BEGIN_NAMESPACE

//----------------------------------------------------------------------------
// Event
//----------------------------------------------------------------------------



Parser::Event::Event()
{
	j_ptr = 0;
}

Parser::Event::Event( const Parser::Event &e )
{
	e.j_ptr->ref.ref();
	j_ptr = e.j_ptr;
}

Parser::Event &Parser::Event::operator=( const Parser::Event &e )
{
	e.j_ptr->ref.ref();
	if( j_ptr && !j_ptr->ref.deref() )
		delete j_ptr;
	j_ptr = e.j_ptr;
	return *this;
}

Parser::Event::~Event()
{
	if( j_ptr && !j_ptr->ref.deref() )
		delete j_ptr;
}


Parser::Event::Type Parser::Event::type()
{
	J_D(Parser::Event);
	return j->type;
}

const QString &Parser::Event::nsprefix( const QString &s ) const
{
	J_D(const Parser::Event);
	QStringList::ConstIterator it = j->nsnames.begin();
	QStringList::ConstIterator it2 = j->nsvalues.begin();
	for(; it != j->nsnames.end(); ++it) {
		if((*it) == s)
			return (*it2);
		++it2;
	}
	static const QString empty_str;
	return empty_str;
}

const QString &Parser::Event::namespaceURI() const
{
	J_D(const Parser::Event);
	return j->ns;
}

const QString &Parser::Event::localName() const
{
	J_D(const Parser::Event);
	return j->ln;
}

const QString &Parser::Event::qName() const
{
	J_D(const Parser::Event);
	return j->qn;
}

const QXmlAttributes &Parser::Event::atts() const
{
	J_D(const Parser::Event);
	return j->a;
}

const QDomElement &Parser::Event::element() const
{
	J_D(const Parser::Event);
	return j->e;
}

const QString Parser::Event::actualString() const
{
	J_D(const Parser::Event);
	return j->str;
}

void Parser::Event::setDocumentOpen( const QString &namespaceURI, const QString &localName, const QString &qName,
					  const QXmlAttributes &atts, const QStringList &nsnames, const QStringList &nsvalues )
{
	if(!j_ptr)
		j_ptr = new Parser::EventPrivate;
	J_D(Parser::Event);
	j->type = DocumentOpen;
	j->ns = namespaceURI;
	j->ln = localName;
	j->qn = qName;
	j->a = atts;
	j->nsnames = nsnames;
	j->nsvalues = nsvalues;
}

void Parser::Event::setDocumentClose( const QString &namespaceURI, const QString &localName, const QString &qName )
{
	if(!j_ptr)
		j_ptr = new Parser::EventPrivate;
	J_D(Parser::Event);
	j->type = DocumentClose;
	j->ns = namespaceURI;
	j->ln = localName;
	j->qn = qName;
}

void Parser::Event::setElement( const QDomElement &elem )
{
	if(!j_ptr)
		j_ptr = new Parser::EventPrivate;
	J_D(Parser::Event);
	j->type = Element;
	j->e = elem;
}

void Parser::Event::setError()
{
	if(!j_ptr)
		j_ptr = new Parser::EventPrivate;
	J_D(Parser::Event);
	j->type = Error;
}

void Parser::Event::setActualString( const QString &str )
{
	if(!j_ptr)
		j_ptr = new Parser::EventPrivate;
	J_D(Parser::Event);
	j->str = str;
}

bool Parser::Event::isNull() const
{
	J_D(const Parser::Event);
	return j == 0;
}

//----------------------------------------------------------------------------
// Parser
//----------------------------------------------------------------------------


Parser::Parser() : QObject(0)
{
	j_ptr = new ParserPrivate(this);
}

Parser::~Parser()
{
	delete j_ptr;
}

void Parser::reset()
{
	j_ptr->reset();
}

void Parser::appendData(const QByteArray &a)
{
	j_ptr->in->appendData(a);

	// if handler was waiting for more, give it a kick
	if(j_ptr->handler->needMore)
		j_ptr->handler->checkNeedMore();
}

Parser::Event Parser::readNext()
{
	Event e;
	if(j_ptr->handler->needMore)
		return e;
	Event *ep = j_ptr->handler->takeEvent();
	if(!ep) {
		if(!j_ptr->reader->parseContinue()) {
			e.setError();
			return e;
		}
		ep = j_ptr->handler->takeEvent();
		if(!ep)
			return e;
	}
	e = *ep;
	delete ep;
	return e;
}

QByteArray Parser::unprocessed() const
{
	return j_ptr->in->unprocessed();
}

QString Parser::encoding() const
{
	return j_ptr->in->encoding();
}

J_END_NAMESPACE
