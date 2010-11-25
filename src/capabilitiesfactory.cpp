/****************************************************************************
 *  capabilitiesfactory.cpp
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

#include "capabilitiesfactory_p.h"
#include <QStringList>

namespace jreen {

CapabilitesFactory::CapabilitesFactory()
{
}

CapabilitesFactory::~CapabilitesFactory()
{
}

QStringList CapabilitesFactory::features() const
{
}

bool CapabilitesFactory::canParse(const QStringRef &name, const QStringRef &uri,
								  const QXmlStreamAttributes &attributes)
{
}

void CapabilitesFactory::handleStartElement(const QStringRef &name, const QStringRef &uri,
											const QXmlStreamAttributes &attributes)
{
}

void CapabilitesFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
}

void CapabilitesFactory::handleCharacterData(const QStringRef &text)
{
}

void CapabilitesFactory::serialize(StanzaExtension *extension, QXmlStreamWriter *writer)
{
}

StanzaExtension::Ptr CapabilitesFactory::createExtension()
{
}

} // namespace jreen
