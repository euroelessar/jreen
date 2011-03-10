/****************************************************************************
 *  multimediadatafactory.h
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

#ifndef MULTIMEDIADATAFACTORY_H
#define MULTIMEDIADATAFACTORY_P_H
#include "multimediadata.h"

namespace Jreen {

class MultimediaDataFactory : public XmlStreamParser
{
public:
    MultimediaDataFactory();
	virtual ~MultimediaDataFactory();
	QStringList features() const;
	bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleEndElement(const QStringRef &name, const QStringRef &uri);
	void handleCharacterData(const QStringRef &text);
	void serialize(const MultimediaData &media, QXmlStreamWriter *writer);
	MultimediaData create();
private:
	QVariantMap m_attributes;
	QVariantList m_data;
	QVariantMap m_currentDataItem;
	int m_depth;
	MultimediaData::Type m_type;
};

} // namespace Jreen

#endif // MULTIMEDIADATAFACTORY_H
