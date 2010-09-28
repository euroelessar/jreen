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

namespace jreen
{

J_STRING(text)
J_STRING(error)

static const QStringList error_types = QStringList()
											<< QLatin1String("auth") << QLatin1String("cancel")
											<< QLatin1String("continue") << QLatin1String("modify")
											<< QLatin1String("wait");

static const QStringList error_conditions = QStringList()
											<< QLatin1String("bad-request") << QLatin1String("conflict")
											<< QLatin1String("feature-not-implemented") << QLatin1String("forbidden")
											<< QLatin1String("gone") << QLatin1String("internal-server-error")
											<< QLatin1String("item-not-found") << QLatin1String("jid-malformed")
											<< QLatin1String("not-acceptable") << QLatin1String("not-allowed")
											<< QLatin1String("not-authorized") << QLatin1String("not-modified")
											<< QLatin1String("payment-required") << QLatin1String("recipient-unavailable")
											<< QLatin1String("redirect") << QLatin1String("registration-required")
											<< QLatin1String("remote-server-not-found") << QLatin1String("remote-server-timeout")
											<< QLatin1String("resource-constraint") << QLatin1String("service-unavailable")
											<< QLatin1String("subscription-required") << QLatin1String("undefined-condition")
											<< QLatin1String("unexpected-request") << QLatin1String("unknown-sender");

//static const QStringList error_conditions = QStringList()
//											<< QLatin1String("bad-format") << QLatin1String("bad-namespace-prefix")
//											<< QLatin1String("conflict") << QLatin1String("connection-timeout")
//											<< QLatin1String("host-gone") << QLatin1String("host-unknown")
//											<< QLatin1String("improper-addressing") << QLatin1String("internal-server-error")
//											<< QLatin1String("invalid-from") << QLatin1String("invalid-id")
//											<< QLatin1String("invalid-namespace") << QLatin1String("invalid-xml")
//											<< QLatin1String("not-authorized") << QLatin1String("policy-violation")
//											<< QLatin1String("remote-connection-failed") << QLatin1String("resource-constraint")
//											<< QLatin1String("restricted-xml") << QLatin1String("see-other-host")
//											<< QLatin1String("system-shutdown") << QLatin1String("undefined-condition")
//											<< QLatin1String("unsupported-encoding") << QLatin1String("unsupported-stanza-type")
//											<< QLatin1String("unsupported-version") << QLatin1String("xml-not-well-formed");

struct ErrorPrivate
{
	Error::Type type;
	Error::Condition condition;
	LangMap text;
	QDomElement app_element;
};

Error::Error(const QDomElement &node) : d_ptr(new ErrorPrivate)
{
	Q_D(Error);
	if(node.isNull())
		return;
	d->condition = Undefined;
	int type = error_types.indexOf(node.attribute(ConstString::type));
	d->type = type < 0 ? UndefinedType : static_cast<Type>(type);
	forelements(const QDomElement &elem, node)
	{
		QString name = elem.nodeName();
		int condition = error_conditions.indexOf(name);
		if(condition > -1)
			d->condition = static_cast<Condition>(condition);
		else if(name == text_str)
			d->text.insert(elem.attribute(ConstString::lang), elem.text());
		else
			d->app_element = elem;
	}
}

Error::Error(Type type, Condition condition, const QDomElement &app_element) : d_ptr(new ErrorPrivate)
{
	Q_D(Error);
	d->type = type;
	d->condition = condition;
	d->app_element = app_element;
}

Error::~Error()
{
}

QDomElement Error::node(QDomDocument *document) const
{
	static const QString stanzas_str("urn:ietf:params:xml:ns:xmpp-stanzas");
	Q_D(const Error);
	if(d->type == UndefinedType || d->condition == Undefined)
		return QDomElement();
	QDomElement node = createElement(document, error_str);
	node.setAttribute(ConstString::type, error_str.at(d->type));
	QDomElement error = createElement(document, error_conditions.at(d->condition));
	error.setAttribute(ConstString::xmlns, stanzas_str);
	d->text.fillNode(node, text_str, stanzas_str);
	return node;
}

}
