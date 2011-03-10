/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2011 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#ifndef PRIVACYQUERYFACTORY_P_H
#define PRIVACYQUERYFACTORY_P_H

#include "privacymanager_p.h"

namespace Jreen
{
class PrivacyQueryFactory : public StanzaExtensionFactory<PrivacyQuery>
{
public:
	PrivacyQueryFactory();
	virtual ~PrivacyQueryFactory();
	QStringList features() const;
	bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleEndElement(const QStringRef &name, const QStringRef &uri);
	void handleCharacterData(const QStringRef &text);
	void serialize(StanzaExtension *extension, QXmlStreamWriter *writer);
	StanzaExtension::Ptr createExtension();
private:
	int m_depth;
	enum State { AtNowhere, AtList, AtItem } m_state;
	QScopedPointer<PrivacyQuery> m_query;
	QString m_name;
	QList<PrivacyItem> m_items;
};

} // namespace Jreen

#endif // PRIVACYQUERYFACTORY_P_H
