/****************************************************************************
**
** Jreen
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
**
*****************************************************************************
**
** $JREEN_BEGIN_LICENSE$
** Jreen is free software: you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** Jreen is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with Jreen.  If not, see <http://www.gnu.org/licenses/>.
** $JREEN_END_LICENSE$
**
****************************************************************************/

#ifndef JREEN_ACTIVITYFACTORY_P_H
#define JREEN_ACTIVITYFACTORY_P_H

#include "stanzaextension.h"
#include "activity.h"

namespace Jreen {

class JREEN_AUTOTEST_EXPORT ActivityFactory : public PayloadFactory<Activity>
{
public:
	ActivityFactory();
	virtual ~ActivityFactory();
	QStringList features() const;
	bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleEndElement(const QStringRef &name, const QStringRef &uri);
	void handleCharacterData(const QStringRef &text);
	void serialize(Payload *extension, QXmlStreamWriter *writer);
	Payload::Ptr createPayload();
	static QLatin1String generalName(Activity::General general);
	static Activity::General generalByName(const QStringRef &general);
	static QLatin1String specificName(Activity::Specific specific);
	static Activity::Specific specificByName(const QStringRef &specific);
private:
	void clear();
	enum State { AtNowhere, AtText, AtType};
	int m_depth;
	State m_state;
	Activity::General m_general;
	Activity::Specific m_specific;
	QString m_text; //may be need to replace by langmap?
};

} // namespace Jreen

#endif // JREEN_ACTIVITYFACTORY_P_H
