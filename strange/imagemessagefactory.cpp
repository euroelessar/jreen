/****************************************************************************
 *  imagemessagefactory.cpp
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

#include "imagemessagefactory_p.h"

namespace jreen {

ImageMessageFactory::ImageMessageFactory();
ImageMessageFactory::~ImageMessageFactory();
QStringList ImageMessageFactory::features() const;
bool ImageMessageFactory::canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
void ImageMessageFactory::handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
void ImageMessageFactory::handleEndElement(const QStringRef &name, const QStringRef &uri);
void ImageMessageFactory::handleCharacterData(const QStringRef &text);
void ImageMessageFactory::serialize(StanzaExtension *extension, QXmlStreamWriter *writer);
StanzaExtension::Ptr ImageMessageFactory::createExtension();

} // namespace jreen
