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

#ifndef ERRORFACTORY_P_H
#define ERRORFACTORY_P_H
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

#endif // ERRORFACTORY_P_H
