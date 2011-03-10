/****************************************************************************
 *  capabilitiesfactory_p.h
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

#ifndef CAPABILITIESFACTORY_P_H
#define CAPABILITIESFACTORY_P_H

#include "stanzaextension.h"
#include "capabilities.h"

namespace Jreen {

class Disco;
class CapabilitesFactory : public StanzaExtensionFactory<Capabilities>
{
public:
	CapabilitesFactory(Disco *disco);
	virtual ~CapabilitesFactory();
	QStringList features() const;
	bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleEndElement(const QStringRef &name, const QStringRef &uri);
	void handleCharacterData(const QStringRef &text);
	void serialize(StanzaExtension *extension, QXmlStreamWriter *writer);
	StanzaExtension::Ptr createExtension();
	static QString hashValue(Disco *disco);
private:	
	Disco *m_disco;
	QString m_ver;
	QString m_node;
};

} // namespace Jreen

#endif // CAPABILITIESFACTORY_P_H
