/****************************************************************************
 *  iq.cpp
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

#include "iq_p.h"
#include "stanza_p.h"
#include <QStringList>

namespace Jreen
{

IQ::IQ(Type type, const JID& to, const QString& id) : Stanza(*new IQPrivate)
{
	Q_D(IQ);
	d->subtype = type;
	d->to = to;
	d->id = id;
}

IQ::IQ() : Stanza(*new IQPrivate)
{
	Q_D(IQ);
	d->subtype = IQ::Invalid;
}

IQ::IQ(IQPrivate &p) : Stanza(p)
{
}

IQ::Type IQ::subtype() const
{
	Q_D(const IQ);
	return d->subtype;
}

void IQ::accept() const
{
	Q_D(const IQ);
	d->accepted = true;
}

bool IQ::accepted() const
{
	Q_D(const IQ);
	return d->accepted;
}

}
