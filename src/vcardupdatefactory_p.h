/****************************************************************************
 *  vcardupdatefactory_p.h
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

#ifndef VCARDUPDATEFACTORY_P_H
#define VCARDUPDATEFACTORY_P_H
#include "vcardupdate.h"

namespace Jreen {

class VCardUpdateFactory : public StanzaExtensionFactory<VCardUpdate>
{
public:
    VCardUpdateFactory();
	virtual ~VCardUpdateFactory();
	QStringList features() const;
	bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleEndElement(const QStringRef &name, const QStringRef &uri);
	void handleCharacterData(const QStringRef &text);
	void serialize(StanzaExtension *extension, QXmlStreamWriter *writer);
	StanzaExtension::Ptr createExtension();
private:
	QString m_hash;
	bool m_hasPhoto;
	int m_depth;
	bool m_isPhoto;
};

} // namespace Jreen

#endif // VCARDUPDATEFACTORY_P_H
