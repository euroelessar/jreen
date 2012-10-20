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

#include "tunefactory_p.h"
#include "jstrings.h"
#include <QXmlStreamWriter>
#include <QUrl>
#include "logger.h"
#include <QStringList>

#define NS_TUNE QLatin1String("http://jabber.org/protocol/tune")

namespace Jreen
{
	static const char *tune_types[] = {
		"artist",
		"length",
		"rating",
		"source",
		"title",
		"track",
		"uri"
	};
	
	enum TuneValue
	{
		TuneInvalid = -1,
		TuneArtist,
		TuneLength,
		TuneRating,
		TuneSource,
		TuneTitle,
		TuneTrack,
		TuneUri,
		TuneCount
	};
	
	TuneFactory::TuneFactory()
	{
		m_depth = 0;
		m_state = -1;
	}
	
	QStringList TuneFactory::features() const
	{
		return QStringList(NS_TUNE);
	}
	
	bool TuneFactory::canParse(const QStringRef &name, const QStringRef &uri,
							   const QXmlStreamAttributes &)
	{
		return name == QLatin1String("tune") && uri == NS_TUNE;
	}
	
	void TuneFactory::handleStartElement(const QStringRef &name, const QStringRef &,
										 const QXmlStreamAttributes &)
	{
		m_depth++;
		if (m_depth == 1)
			m_data.fill(QString(), TuneCount);
		else if(m_depth == 2)
			m_state = strToEnum(name, tune_types);
		//		m_subtype = strToEnum<Tune::Type>(name,tune_types);
	}
	
	void TuneFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
	{
		Q_UNUSED(name);
		Q_UNUSED(uri);
		m_depth--;
	}
	
	void TuneFactory::handleCharacterData(const QStringRef &text)
	{
		if(m_depth == 2 && m_state != TuneInvalid)
			m_data[m_state] = text.toString();
	}
	
#define WRITE_ELEMENT(property, checker, converter) \
	if (tune->property() checker) \
		writer->writeTextElement(QLatin1String( #property ), converter (tune->property()))
	
#define WRITE_TEXT_ELEMENT(property) \
	WRITE_ELEMENT(property, .length() > 0, )
	
	inline QString stringFromUriHelper(const QUrl &uri)
	{ return QLatin1String(uri.toEncoded()); }
		
	void TuneFactory::serialize(Payload *extension, QXmlStreamWriter *writer)
	{
		Tune *tune = se_cast<Tune*>(extension);
		writer->writeStartElement(QLatin1String("tune"));
		writer->writeDefaultNamespace(NS_TUNE);
		WRITE_TEXT_ELEMENT(artist);
		WRITE_ELEMENT(length, > -1, QString::number);
		WRITE_ELEMENT(rating, > -1, QString::number);
		WRITE_TEXT_ELEMENT(source);
		WRITE_TEXT_ELEMENT(title);
		WRITE_TEXT_ELEMENT(track);
		WRITE_ELEMENT(uri, .isValid(), stringFromUriHelper);
		writer->writeEndElement();
	}
	
	Payload::Ptr TuneFactory::createPayload()
	{
		Tune *tune = new Tune();
		bool ok = true;
		tune->setArtist(m_data[TuneArtist]);
		tune->setLength(m_data[TuneLength].toInt(&ok) * ok + ok - 1);
		tune->setRating(m_data[TuneRating].toInt(&ok) * ok + ok - 1);
		tune->setSource(m_data[TuneSource]);
		tune->setTitle(m_data[TuneTitle]);
		tune->setTrack(m_data[TuneTrack]);
		tune->setUri(QUrl::fromUserInput(m_data[TuneUri]));
		return Payload::Ptr(tune);
	}
} // namespace Jreen
