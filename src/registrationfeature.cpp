/****************************************************************************
**
** Jreen
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "registrationfeature_p.h"
#include "registrationmanager_p.h"

namespace Jreen
{

#define NS_REGISTER QLatin1String("http://jabber.org/features/iq-register")

RegistrationFeature::RegistrationFeature(RegistrationManager *manager)
    : StreamFeature(RegistrationRequest), m_manager(manager), m_activatable(false)
{
}

int RegistrationFeature::priority()
{
	return 10;
}

bool RegistrationFeature::isActivatable()
{
	return m_activatable;
}

bool RegistrationFeature::activate()
{
	RegistrationManagerPrivate::get(m_manager)->handleConnection();
	return true;
}

void RegistrationFeature::reset()
{
	m_activatable = false;
}

bool RegistrationFeature::canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(attributes);
	return name == QLatin1String("register") && uri == NS_REGISTER;
}

void RegistrationFeature::handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(uri);
	Q_UNUSED(attributes);
	if (name == QLatin1String("register"))
		m_activatable = true;
}

void RegistrationFeature::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	Q_UNUSED(name);
	Q_UNUSED(uri);
}

void RegistrationFeature::handleCharacterData(const QStringRef &text)
{
	Q_UNUSED(text);
}

} // namespace Jreen
