/****************************************************************************
 *  activityfactory_p.h
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

#ifndef ACTIVITYFACTORY_P_H
#define ACTIVITYFACTORY_P_H
#include "activity.h"

namespace jreen {

class ActivityFactory : public StanzaExtensionFactory<Activity>
{
public:
	ActivityFactory();
	virtual ~ActivityFactory();
	QStringList features() const;
	bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleEndElement(const QStringRef &name, const QStringRef &uri);
	void handleCharacterData(const QStringRef &text);
	void serialize(StanzaExtension *extension, QXmlStreamWriter *writer);
	StanzaExtension::Ptr createExtension();
private:
	void clear();
	enum State {AtText,AtType};
	int m_depth;
	State m_state;
	Activity::General m_general;
	int m_specific;
	QString m_text; //may be need to replace by langmap?
};

} // namespace jreen

#endif // ACTIVITYFACTORY_P_H
