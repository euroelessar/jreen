/****************************************************************************
**
** Jreen
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef JREEN_REGISTRATIONFEATURE_P_H
#define JREEN_REGISTRATIONFEATURE_P_H

#include "streamfeature_p.h"

namespace Jreen
{

class RegistrationManager;

class RegistrationFeature : public StreamFeature
{
public:
	RegistrationFeature(RegistrationManager *manager);
	
	virtual int priority();
	virtual bool isActivatable();
	virtual bool activate();
	virtual void reset();
	virtual bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	virtual void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	virtual void handleEndElement(const QStringRef &name, const QStringRef &uri);
	virtual void handleCharacterData(const QStringRef &text);
	
private:
	RegistrationManager *m_manager;
	bool m_activatable;
};

} // namespace Jreen

#endif // JREEN_REGISTRATIONFEATURE_P_H
