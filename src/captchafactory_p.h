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

#ifndef JREEN_CAPTCHAFACTORY_P_H
#define JREEN_CAPTCHAFACTORY_P_H

#include "captcha.h"
#include "dataformfactory_p.h"

namespace Jreen {

class JREEN_AUTOTEST_EXPORT CaptchaFactory : public PayloadFactory<Captcha>
{
public:
	CaptchaFactory();
	
	QStringList features() const;
	bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleEndElement(const QStringRef &name, const QStringRef &uri);
	void handleCharacterData(const QStringRef &text);
	void serialize(Payload *extension, QXmlStreamWriter *writer);
	Payload::Ptr createPayload();
	
private:
	int m_depth : 31;
	int m_atFactory : 1;
	DataFormFactory m_factory;
	QScopedPointer<Captcha> m_captcha;
};

} // namespace Jreen

#endif // JREEN_CAPTCHAFACTORY_P_H
