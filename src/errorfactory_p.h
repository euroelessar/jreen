/****************************************************************************
**
** Jreen
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#ifndef JREEN_ERRORFACTORY_P_H
#define JREEN_ERRORFACTORY_P_H
#include "error.h"
#include "stanzaextension.h"

namespace Jreen {

class JREEN_AUTOTEST_EXPORT ErrorFactory : public PayloadFactory<Error>
{
public:
    ErrorFactory();
    virtual ~ErrorFactory();
    virtual bool canParse(const QStringRef& name, const QStringRef& uri, const QXmlStreamAttributes& attributes);
    virtual Payload::Ptr createPayload();
    virtual QStringList features() const;
    virtual void handleStartElement(const QStringRef& name, const QStringRef& uri, const QXmlStreamAttributes& attributes);    
    virtual void handleCharacterData(const QStringRef& text);
    virtual void handleEndElement(const QStringRef& name, const QStringRef& uri);
    virtual void serialize(Payload* obj, QXmlStreamWriter* writer);
private:
	enum State {AtCondition,AtText};
	State m_state;
	int m_depth;
	QString m_text;
	Error::Type m_type;
	Error::Condition m_condition;
};

} // namespace Jreen

#endif // JREEN_ERRORFACTORY_P_H
