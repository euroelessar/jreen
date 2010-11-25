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

J_STRING(iq)

static const QStringList iq_types = QStringList() << QLatin1String("get") << QLatin1String("set")
									<< QLatin1String("result") << QLatin1String("error");

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
	if(node.nodeName() != iq_str)
	{
		d->subtype = Invalid;
		return;
	}
	int type = iq_types.indexOf(node.attribute(QLatin1String("type")));
	d->subtype = type < 0 ? Invalid : static_cast<Type>(type);
}

IQ::Type IQ::subtype() const
{
	Q_D(const IQ);
	return d->subtype;
}

void IQ::writeXml(QXmlStreamWriter *writer) const
{
	Q_D(const IQ);
	writer->writeStartElement(iq_str);
	d->setAttributes(writer);
	if (d->subtype != Invalid) {
		writer->writeAttribute(QLatin1String("type"), iq_types.at(d->subtype));
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
