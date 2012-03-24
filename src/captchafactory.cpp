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

#include "captchafactory_p.h"

#define NS_CAPTCHA QLatin1String("urn:xmpp:captcha")

namespace Jreen {

CaptchaFactory::CaptchaFactory() : m_depth(0), m_atFactory(0)
{
}

QStringList CaptchaFactory::features() const
{
	return QStringList();
}

bool CaptchaFactory::canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(attributes);
	return name == QLatin1String("captcha") && uri == NS_CAPTCHA;
}

void CaptchaFactory::handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	++m_depth;

	if (m_depth == 1)
		m_captcha.reset(new Captcha);
	else if (m_depth == 2 && m_factory.canParse(name, uri, attributes))
		m_atFactory = true;

	if (m_atFactory)
		m_factory.handleStartElement(name, uri, attributes);
}

void CaptchaFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	if (m_atFactory) {
		m_factory.handleEndElement(name, uri);
		if (m_depth == 2) {
			DataForm::Ptr form = m_factory.createPayload().staticCast<DataForm>();
			if (form->typeName() == NS_CAPTCHA)
				m_captcha->setForm(form);
			m_atFactory = false;
		}
	}

	--m_depth;
}

void CaptchaFactory::handleCharacterData(const QStringRef &text)
{
	if (m_atFactory)
		m_factory.handleCharacterData(text);
}

void CaptchaFactory::serialize(Payload *extension, QXmlStreamWriter *writer)
{
	Captcha *captcha = payload_cast<Captcha*>(extension);
	writer->writeStartElement(QLatin1String("captcha"));
	writer->writeDefaultNamespace(NS_CAPTCHA);
	if (captcha->form())
		m_factory.serialize(captcha->form().data(), writer);
	writer->writeEndElement();
}

Payload::Ptr CaptchaFactory::createPayload()
{
	return Payload::Ptr(m_captcha.take());
}

} // namespace Jreen
