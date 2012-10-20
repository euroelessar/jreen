/****************************************************************************
**
** Jreen
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
**
*****************************************************************************
**
** $JREEN_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $JREEN_END_LICENSE$
**
****************************************************************************/

#ifndef MOODFACTORY_P_H
#define MOODFACTORY_P_H
#include "stanzaextension.h"
#include "mood.h"
#include "langmap.h"

namespace Jreen {

class JREEN_AUTOTEST_EXPORT MoodFactory : public PayloadFactory<Mood>
{
public:
	MoodFactory();
	virtual ~MoodFactory();
	QStringList features() const;
	bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleEndElement(const QStringRef &name, const QStringRef &uri);
	void handleCharacterData(const QStringRef &text);
	void serialize(Payload *extension, QXmlStreamWriter *writer);
	Payload::Ptr createPayload();
	
	static QLatin1String typeName(Mood::Type type);
	static Mood::Type typeByName(const QStringRef &name);
private:
	int m_depth;
	Mood::Type m_subtype;
	QString m_text; //may be need to replace by langmap?
};

} // namespace Jreen

#endif // MOODFACTORY_P_H
