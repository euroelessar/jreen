/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#include "zlibcompressionfeature.h"
#include "zlibdatastream.h"
#include <QXmlStreamWriter>

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
	return uri == QLatin1String("http://jabber.org/features/compress") || uri == QLatin1String("http://jabber.org/protocol/compress");
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
	writer->writeDefaultNamespace(QLatin1String("http://jabber.org/protocol/compress"));
	writer->writeTextElement(QLatin1String("method"), QLatin1String("zlib"));
	writer->writeEndElement();
	return true;
}
}
