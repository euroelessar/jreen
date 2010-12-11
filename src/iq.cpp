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

namespace jreen
{

static const char *iq_types[] = {"get",
								 "set",
								 "result",
								 "error"
								};

IQ::IQ(Type type, const JID& to, const QString& id) : Stanza(*new IQPrivate)
{
	Q_D(IQ);
	d->subtype = type;
	d->to = to;
	d->id = id;
}

IQ::IQ(IQPrivate &p) : Stanza(p)
{
}

IQ::IQ(const QDomElement &node) : Stanza(node, new IQPrivate)
{
	Q_D(IQ);
	if(node.nodeName() != QLatin1String("iq"))
	{
		d->subtype = Invalid;
		return;
	}
	d->subtype = strToEnum<IQ::Type>(node.attribute(QLatin1String("type")),iq_types);
}

IQ::Type IQ::subtype() const
{
	Q_D(const IQ);
	return d->subtype;
}

void IQ::writeXml(QXmlStreamWriter *writer) const
{
	Q_D(const IQ);
	writer->writeStartElement(QLatin1String("iq"));
	d->setAttributes(writer);
	if (d->subtype != Invalid) {
		writer->writeAttribute(QLatin1String("type"), enumToStr(d->subtype,iq_types));
		d->addExtensions(writer);
	}
	writer->writeEndElement();
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
