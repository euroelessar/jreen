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

namespace jreen
{

J_STRING(text)
J_STRING(error)

const char *error_types[] = {"auth","cancel",
							 "continue","modify",
							 "wait"};

const char *error_conditions[] = {"bad-request","conflict",
								  "feature-not-implemented", "forbidden",
								  "gone", "internal-server-error",
								  "item-not-found", "jid-malformed",
								  "not-acceptable", "not-allowed",
								  "not-authorized", "not-modified",
								  "payment-required", "recipient-unavailable",
								  "redirect", "registration-required",
								  "remote-server-not-found", "remote-server-timeout",
								  "resource-constraint", "service-unavailable",
								  "subscription-required", "undefined-condition",
								  "unexpected-request", "unknown-sender"};



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
};

Error::Error(Error::Type type, Error::Condition condition, const jreen::LangMap& string)
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

//void Error::writeXml(QXmlStreamWriter *writer) const
//{
//	static const QString stanzas_str("urn:ietf:params:xml:ns:xmpp-stanzas");
//	Q_D(const Error);
//	if(d->type == UndefinedType || d->condition == Undefined)
//		return;
//	writer->writeStartElement(error_str);
//	writer->writeAttribute(ConstString::type, error_str.at(d->type));
//	writer->writeEmptyElement(error_conditions.at(d->condition));
//	writer->writeAttribute(ConstString::xmlns, stanzas_str);
//	d->text.fillNode(writer, text_str, stanzas_str);
//	writer->writeEndElement();
//}

}
