/****************************************************************************
 *  moodfactory.cpp
 *
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#include "moodfactory_p.h"
#include "jstrings.h"
#include <QXmlStreamWriter>

#define NS_MOOD QLatin1String("http://jabber.org/protocol/mood")

static const char *mood_types[] = {
	"afraid",
	"amazed",
	"angry",
	"amorous",
	"annoyed",
	"anxious",
	"aroused",
	"ashamed",
	"bored",
	"brave",
	"calm",
	"cautious",
	"cold",
	"confident",
	"confused",
	"contemplative",
	"contented",
	"cranky",
	"crazy",
	"creative",
	"curious",
	"dejected",
	"depressed",
	"disappointed",
	"disgusted",
	"dismayed",
	"distracted",
	"embarrassed",
	"envious",
	"excited",
	"flirtatious",
	"frustrated",
	"grumpy",
	"guilty",
	"happy",
	"hopeful",
	"hot",
	"humbled",
	"humiliated",
	"hungry",
	"hurt",
	"impressed",
	"in_awe",
	"in_love",
	"indignant",
	"interested",
	"intoxicated",
	"invincible",
	"jealous",
	"lonely",
	"lucky",
	"mean",
	"moody",
	"nervous",
	"neutral",
	"offended",
	"outraged",
	"playful",
	"proud",
	"relaxed",
	"relieved",
	"remorseful",
	"restless",
	"sad",
	"sarcastic",
	"serious",
	"shocked",
	"shy",
	"sick",
	"sleepy",
	"spontaneous",
	"stressed",
	"strong",
	"surprised",
	"thankful",
	"thirsty",
	"tired",
	"undefined",
	"weak",
	"worried"
};

namespace jreen {

MoodFactory::MoodFactory()
{
	m_depth = 0;
}

MoodFactory::~MoodFactory()
{

}

QStringList MoodFactory::features() const
{
	return QStringList(NS_MOOD);
}

bool MoodFactory::canParse(const QStringRef &name, const QStringRef &uri,
						   const QXmlStreamAttributes &)
{
	bool can = name == QLatin1String("mood") && uri == NS_MOOD;
	if(can)
		m_text.clear();
	return can;
}

void MoodFactory::handleStartElement(const QStringRef &name, const QStringRef &,
									 const QXmlStreamAttributes &)
{
	m_depth++;
	if(m_depth == 1 && name != QLatin1String("text"))
		m_subtype = strToEnum<Mood::Type>(name,mood_types);
}

void MoodFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	Q_UNUSED(name);
	Q_UNUSED(uri);
	m_depth--;
}

void MoodFactory::handleCharacterData(const QStringRef &text)
{
	if(m_depth == 2)
		m_text = text.toString();
}

void MoodFactory::serialize(StanzaExtension *extension, QXmlStreamWriter *writer)
{
	Mood *mood = se_cast<Mood*>(extension);
	if(mood->subtype() == Mood::Invalid)
		return;
	writer->writeStartElement(QLatin1String("mood"));
	writer->writeDefaultNamespace(NS_MOOD);
	writer->writeEmptyElement(enumToStr(m_subtype,mood_types));
	if(!m_text.isEmpty())
		writer->writeTextElement(QLatin1String("text"),m_text);
}

StanzaExtension::Ptr MoodFactory::createExtension()
{
	return StanzaExtension::Ptr(new Mood(m_subtype,m_text));
}

} // namespace jreen
