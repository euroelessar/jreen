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

#include "pgpfactory_p.h"
#include <QStringList>

#define NS_SIGNED QLatin1String("jabber:x:signed")
#define NS_ENCRYPTED QLatin1String("jabber:x:encrypted")

namespace Jreen
{

PGPSignedFactory::PGPSignedFactory() : m_depth(0)
{
}

QStringList PGPSignedFactory::features() const
{
	return QStringList(NS_SIGNED);
}

bool PGPSignedFactory::canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(attributes);
	return name == QLatin1String("x") && uri == NS_SIGNED;
}

void PGPSignedFactory::handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(name);
	Q_UNUSED(uri);
	Q_UNUSED(attributes);
	m_depth++;
	if (m_depth == 1)
		m_query.reset(new PGPSigned);
}

void PGPSignedFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	Q_UNUSED(name);
	Q_UNUSED(uri);
	--m_depth;
}

void PGPSignedFactory::handleCharacterData(const QStringRef &text)
{
	Q_UNUSED(text);
	if (m_depth == 1)
		m_query->setSignature(text.toString());
}

void PGPSignedFactory::serialize(Payload *extension, QXmlStreamWriter *writer)
{
	PGPSigned *query = payload_cast<PGPSigned*>(extension);
	writer->writeStartElement(QLatin1String("x"));
	writer->writeDefaultNamespace(NS_SIGNED);
	writer->writeCharacters(query->signature());
	writer->writeEndElement();
}

Payload::Ptr PGPSignedFactory::createPayload()
{
	return Payload::Ptr(m_query.take());
}

PGPEncryptedFactory::PGPEncryptedFactory() : m_depth(0)
{
}

QStringList PGPEncryptedFactory::features() const
{
	return QStringList(NS_ENCRYPTED);
}

bool PGPEncryptedFactory::canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(attributes);
	return name == QLatin1String("x") && uri == NS_ENCRYPTED;
}

void PGPEncryptedFactory::handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(name);
	Q_UNUSED(uri);
	Q_UNUSED(attributes);
	m_depth++;
	if (m_depth == 1)
		m_query.reset(new PGPEncrypted);
}

void PGPEncryptedFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	Q_UNUSED(name);
	Q_UNUSED(uri);
	--m_depth;
}

void PGPEncryptedFactory::handleCharacterData(const QStringRef &text)
{
	Q_UNUSED(text);
	if (m_depth == 1)
		m_query->setEncryptedText(text.toString());
}

void PGPEncryptedFactory::serialize(Payload *extension, QXmlStreamWriter *writer)
{
	PGPEncrypted *query = payload_cast<PGPEncrypted*>(extension);
	writer->writeStartElement(QLatin1String("x"));
	writer->writeDefaultNamespace(NS_ENCRYPTED);
	writer->writeCharacters(query->encryptedText());
	writer->writeEndElement();
}

Payload::Ptr PGPEncryptedFactory::createPayload()
{
	return Payload::Ptr(m_query.take());
}

} // namespace Jreen
