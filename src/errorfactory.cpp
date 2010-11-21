/****************************************************************************
 *  errorfactory.cpp
 *
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
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

#include "errorfactory_p.h"
#include <QXmlStreamWriter>
#include <QStringList>

namespace jreen {

#define NS_ERROR QLatin1String("urn:ietf:params:xml:ns:xmpp-stanzas")
  
ErrorFactory::ErrorFactory()
{
}

bool ErrorFactory::canParse(const QStringRef& name, const QStringRef& uri, const QXmlStreamAttributes& attributes)
{
	return name == QLatin1String("error") && uri == NS_ERROR;
}

StanzaExtension::Ptr ErrorFactory::createExtension()
{
	return StanzaExtension::Ptr(new Error(m_type,m_condition));
}

QStringList ErrorFactory::features() const
{
	return QStringList(NS_ERROR);
}

void ErrorFactory::handleStartElement(const QStringRef& name, const QStringRef& uri, const QXmlStreamAttributes& attributes)
{

}

void ErrorFactory::handleCharacterData(const QStringRef& text)
{

}
void ErrorFactory::handleEndElement(const QStringRef& name, const QStringRef& uri)
{

}

void ErrorFactory::serialize(StanzaExtension* obj, QXmlStreamWriter* writer)
{

}

ErrorFactory::~ErrorFactory()
{

}


} // namespace jreen
