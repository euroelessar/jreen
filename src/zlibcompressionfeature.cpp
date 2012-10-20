/****************************************************************************
**
** Jreen
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "zlibcompressionfeature_p.h"
#include "zlibdatastream_p.h"
#include <QXmlStreamWriter>

#define NS_COMPRESS_FEATURE QLatin1String("http://jabber.org/features/compress")
#define NS_COMPRESS_PROTOCOL QLatin1String("http://jabber.org/protocol/compress")

namespace Jreen
{
ZLibCompressionFeature::ZLibCompressionFeature() : StreamFeature(CompressionLayer)
{
	reset();
}

void ZLibCompressionFeature::reset()
{
	m_state = AtStart;
	m_depth = 0;
	m_methods.clear();
}

bool ZLibCompressionFeature::canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(name);
	Q_UNUSED(attributes);
	return uri == NS_COMPRESS_FEATURE || uri == NS_COMPRESS_PROTOCOL;
}

void ZLibCompressionFeature::handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(uri);
	Q_UNUSED(attributes);
	m_depth++;
	if (m_depth == 1 && name == QLatin1String("compression")) {
		m_state = AtMethods;
	} else if (m_depth == 2 && m_state == AtMethods && name == QLatin1String("method")) {
		m_state = AtMethod;
	}
}

void ZLibCompressionFeature::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	Q_UNUSED(uri);
	if (m_depth == 1) {
		m_state = AtStart;
		if (name == QLatin1String("compressed")) {
			m_info->addDataStream(new ZLibDataStream());
			m_info->completed(StreamInfo::ResendHeader);
		}
	} else if (m_depth == 2 && m_state == AtMethod) {
		m_state = AtMethods;
	}
	m_depth--;
}

void ZLibCompressionFeature::handleCharacterData(const QStringRef &text)
{
	if (m_state == AtMethod)
		m_methods << text.toString();
}

bool ZLibCompressionFeature::isActivatable()
{
	return m_methods.contains(QLatin1String("zlib"));
}

bool ZLibCompressionFeature::activate()
{
	QXmlStreamWriter *writer = m_info->writer();
	writer->writeStartElement(QLatin1String("compress"));
	writer->writeDefaultNamespace(NS_COMPRESS_PROTOCOL);
	writer->writeTextElement(QLatin1String("method"), QLatin1String("zlib"));
	writer->writeEndElement();
	return true;
}
}
