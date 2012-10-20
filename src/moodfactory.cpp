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

#include "moodfactory_p.h"
#include "jstrings.h"
#include <QXmlStreamWriter>
#include "logger.h"

#define NS_MOOD QLatin1String("http://jabber.org/protocol/mood")

static const char *mood_types[] = {
	"afraid",
	"amazed",
	"amorous",
	"angry",
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
	"grateful",
	"grieving",
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
	"lost",
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
	"satisfied",
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

namespace Jreen {

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
	return name == QLatin1String("mood") && uri == NS_MOOD;
}

struct MoodLessThen
{
	bool operator()(const QStringRef &a, const char *b)
	{
		return a.compare(QLatin1String(b)) < 0;
	}
	
	bool operator()(const char *a, const QStringRef &b)
	{
		return b.compare(QLatin1String(a)) > 0;
	}
};

void MoodFactory::handleStartElement(const QStringRef &name, const QStringRef &,
									 const QXmlStreamAttributes &)
{
	m_depth++;
	if (m_depth == 1)
		m_text.clear(), m_subtype = Mood::Empty;
	else if(m_depth == 2 && name != QLatin1String("text"))
		m_subtype = typeByName(name);
//		m_subtype = strToEnum<Mood::Type>(name,mood_types);
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

void MoodFactory::serialize(Payload *extension, QXmlStreamWriter *writer)
{
	Mood *mood = se_cast<Mood*>(extension);
	if(mood->type() == Mood::Invalid)
		return;
	writer->writeStartElement(QLatin1String("mood"));
	writer->writeDefaultNamespace(NS_MOOD);
	if (mood->type() != Mood::Empty) {
		writer->writeEmptyElement(enumToStr(mood->type(), mood_types));
		if(!mood->text().isEmpty())
			writer->writeTextElement(QLatin1String("text"), mood->text());
	}
	writer->writeEndElement();
}

Payload::Ptr MoodFactory::createPayload()
{
	return Payload::Ptr(new Mood(m_subtype, m_text));
}

QLatin1String MoodFactory::typeName(Mood::Type type)
{
	return QLatin1String(type <= Mood::Invalid ? 0 : mood_types[type]);
}

Mood::Type MoodFactory::typeByName(const QStringRef &name)
{
	if (name.isEmpty())
		return Mood::Empty;
	int n = sizeof(mood_types)/sizeof(char*);
	MoodLessThen moodLessThen;
	const char **res = qBinaryFind(mood_types, mood_types + n, name, moodLessThen);
	return static_cast<Mood::Type>((res == mood_types + n) ? -1 : (res - mood_types));
}

} // namespace Jreen
