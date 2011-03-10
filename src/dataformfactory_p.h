/****************************************************************************
 *  dataformfactory_p.h
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

#ifndef DATAFORMFACTORY_P_H
#define DATAFORMFACTORY_P_H
#include "dataform_p.h"

namespace Jreen {

class DataFormFactoryPrivate;
class DataFormFactory : public StanzaExtensionFactory<DataForm>
{
	Q_DECLARE_PRIVATE(DataFormFactory)
public:
	DataFormFactory();
	virtual ~DataFormFactory();
	QStringList features() const;
	bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleEndElement(const QStringRef &name, const QStringRef &uri);
	void handleCharacterData(const QStringRef &text);
	void serialize(StanzaExtension *extension, QXmlStreamWriter *writer);
	StanzaExtension::Ptr createExtension();
private:
	QScopedPointer<DataFormFactoryPrivate> d_ptr;
};

} // namespace Jreen

#endif // DATAFORMFACTORY_P_H
