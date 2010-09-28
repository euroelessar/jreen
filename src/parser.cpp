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

namespace jreen
{

//----------------------------------------------------------------------------
// Event
//----------------------------------------------------------------------------



Parser::Event::Event()
{
	d_ptr = 0;
}

Parser::Event::Event(const Parser::Event &e)
{
	e.d_ptr->ref.ref();
	d_ptr = e.d_ptr;
}

Parser::Event &Parser::Event::operator=(const Parser::Event &e)
{
	e.d_ptr->ref.ref();
	if(d_ptr && !d_ptr->ref.deref())
		delete d_ptr;
	d_ptr = e.d_ptr;
	return *this;
}

Parser::Event::~Event()
{
	if(d_ptr && !d_ptr->ref.deref())
		delete d_ptr;
}


Parser::Event::Type Parser::Event::type()
{
	Q_D(Parser::Event);
	return d->type;
}

const QString &Parser::Event::nsprefix(const QString &s) const
{
	Q_D(const Parser::Event);
	QStringList::ConstIterator it = d->nsnames.begin();
	QStringList::ConstIterator it2 = d->nsvalues.begin();
	for(; it != d->nsnames.end(); ++it) {
		if((*it) == s)
			return (*it2);
		++it2;
	}
	static const QString empty_str;
	return empty_str;
}

const QString &Parser::Event::namespaceURI() const
{
	Q_D(const Parser::Event);
	return d->ns;
}

const QString &Parser::Event::localName() const
{
	Q_D(const Parser::Event);
	return d->ln;
}

const QString &Parser::Event::qName() const
{
	Q_D(const Parser::Event);
	return d->qn;
}

const QXmlAttributes &Parser::Event::atts() const
{
	Q_D(const Parser::Event);
	return d->a;
}

const QDomElement &Parser::Event::element() const
{
	Q_D(const Parser::Event);
	return d->e;
}

const QString Parser::Event::actualString() const
{
	Q_D(const Parser::Event);
	return d->str;
}

void Parser::Event::setDocumentOpen(const QString &namespaceURI, const QString &localName, const QString &qName,
					  const QXmlAttributes &atts, const QStringList &nsnames, const QStringList &nsvalues)
{
	if(!d_ptr)
		d_ptr = new Parser::EventPrivate;
	Q_D(Parser::Event);
	d->type = DocumentOpen;
	d->ns = namespaceURI;
	d->ln = localName;
	d->qn = qName;
	d->a = atts;
	d->nsnames = nsnames;
	d->nsvalues = nsvalues;
}

void Parser::Event::setDocumentClose(const QString &namespaceURI, const QString &localName, const QString &qName)
{
	if(!d_ptr)
		d_ptr = new Parser::EventPrivate;
	Q_D(Parser::Event);
	d->type = DocumentClose;
	d->ns = namespaceURI;
	d->ln = localName;
	d->qn = qName;
}

void Parser::Event::setElement(const QDomElement &elem)
{
	if(!d_ptr)
		d_ptr = new Parser::EventPrivate;
	Q_D(Parser::Event);
	d->type = Element;
	d->e = elem;
}

void Parser::Event::setError()
{
	if(!d_ptr)
		d_ptr = new Parser::EventPrivate;
	Q_D(Parser::Event);
	d->type = Error;
}

void Parser::Event::setActualString(const QString &str)
{
	if(!d_ptr)
		d_ptr = new Parser::EventPrivate;
	Q_D(Parser::Event);
	d->str = str;
}

bool Parser::Event::isNull() const
{
	return d_func() == 0;
}

//----------------------------------------------------------------------------
// Parser
//----------------------------------------------------------------------------


Parser::Parser() : QObject(0), d_ptr(new ParserPrivate(this))
{
}

Parser::~Parser()
{
}

void Parser::reset()
{
	d_ptr->reset();
}

void Parser::appendData(const QByteArray &a)
{
	d_ptr->in->appendData(a);

	// if handler was waiting for more, give it a kick
	if(d_ptr->handler->needMore)
		d_ptr->handler->checkNeedMore();
}

Parser::Event Parser::readNext()
{
	Event e;
	if(d_ptr->handler->needMore)
		return e;
	Event *ep = d_ptr->handler->takeEvent();
	if(!ep) {
		if(!d_ptr->reader->parseContinue()) {
			e.setError();
			return e;
		}
		ep = d_ptr->handler->takeEvent();
		if(!ep)
			return e;
	}
	e = *ep;
	delete ep;
	return e;
}

QByteArray Parser::unprocessed() const
{
	return d_ptr->in->unprocessed();
}

QString Parser::encoding() const
{
	return d_ptr->in->encoding();
}

}
