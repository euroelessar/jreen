/****************************************************************************
 *  error.cpp
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

#include "error.h"
#include "jstrings.h"
#include "langmap.h"
#include <QStringList>
#include <QXmlStreamWriter>

namespace Jreen
{

class ErrorPrivate
{
public:
	Error::Type type;
	Error::Condition condition;
	LangMap text;
};

Error::Error(Error::Type type, Error::Condition condition, const Jreen::LangMap& string)
	: StanzaExtension(), d_ptr(new ErrorPrivate)
{
	Q_D(Error);
	d->type = type;
	d->condition = condition;
	d->text = string;
}

Error::~Error()
{
}

Error::Type Error::type() const
{
	return d_func()->type;
}

Error::Condition Error::condition() const
{
	return d_func()->condition;
}

}
