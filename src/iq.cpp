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

#include "iq.h"
#include "stanza_p.h"
#include <QStringList>

namespace jreen
{

J_STRING(iq)

static const QStringList iq_types = QStringList() << QLatin1String("get") << QLatin1String("set")
									<< QLatin1String("result") << QLatin1String("error");

struct IQPrivate : public StanzaPrivate
{
	IQPrivate() : accepted(false) {}
	IQ::Type subtype;
	mutable bool accepted;
};

IQ::IQ(Type type, const JID& to, const QString& id) : Stanza(new IQPrivate)
{
	Q_D(IQ);
	d->subtype = type;
	d->to = to;
	d->id = id;
}

IQ::IQ(const QDomElement &node) : Stanza(node, new IQPrivate)
{
	Q_D(IQ);
	if(node.nodeName() != iq_str)
	{
		d->subtype = Invalid;
		return;
	}
	int type = iq_types.indexOf(node.attribute(ConstString::type));
	d->subtype = type < 0 ? Invalid : static_cast<Type>(type);
}

IQ::Type IQ::subtype() const
{
	Q_D(const IQ);
	return d->subtype;
}

QDomElement IQ::node() const
{
	Q_D(const IQ);
	if(!d->node.isNull())
		return d->node;
	QDomElement node = DomCreater::instance().createElement(iq_str);
	d->setAttributes(node);
	if(d->subtype == Invalid)
		return node;
	node.setAttribute(ConstString::type, iq_types.at(d->subtype));
	d->addExtensions(node);
	return node;
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
