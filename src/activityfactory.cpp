/****************************************************************************
**
** Jreen
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
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
#include <QXmlStreamWriter>
#include <qstringlist.h>
#include "activityfactory_p.h"
#include "jstrings.h"
#include "util.h"

#define NS_ACTIVITY QLatin1String("http://jabber.org/protocol/activity")

namespace Jreen {

using namespace Util;

static const char* general_types[]= {
	"doing_chores",
	"drinking",
	"eating",
	"exercising",
	"grooming",
	"having_appointment",
	"inactive",
	"relaxing",
	"talking",
	"traveling",
	"undefined",
	"working"
};

static const char* specific_types[]= {
	"at_the_spa",
	"brushing_teeth",
	"buying_groceries",
	"cleaning",
	"coding",
	"commuting",
	"cooking",
	"cycling",
	"dancing",
	"day_off",
	"doing_maintenance",
	"doing_the_dishes",
	"doing_the_laundry",
	"driving",
	"fishing",
	"gaming",
	"gardening",
	"getting_a_haircut",
	"going_out",
	"hanging_out",
	"having_a_beer",
	"having_a_snack",
	"having_breakfast",
	"having_coffee",
	"having_dinner",
	"having_lunch",
	"having_tea",
	"hiding",
	"hiking",
	"in_a_car",
	"in_a_meeting",
	"in_real_life",
	"jogging",
	"on_a_bus",
	"on_a_plane",
	"on_a_train",
	"on_a_trip",
	"on_the_phone",
	"on_vacation",
	"on_video_phone",
	"other",
	"partying",
	"playing_sports",
	"praying",
	"reading",
	"rehearsing",
	"running",
	"running_an_errand",
	"scheduled_holiday",
	"shaving",
	"shopping",
	"skiing",
	"sleeping",
	"smoking",
	"socializing",
	"studying",
	"sunbathing",
	"swimming",
	"taking_a_bath",
	"taking_a_shower",
	"thinking",
	"walking",
	"walking_the_dog",
	"watching_a_movie",
	"watching_tv",
	"working_out",
	"writing"
};

struct ActivityLessThen
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

ActivityFactory::ActivityFactory()
{
	clear();
	m_depth = 0;
}

ActivityFactory::~ActivityFactory()
{
}

QStringList ActivityFactory::features() const
{
	return QStringList(NS_ACTIVITY);
}

bool ActivityFactory::canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(attributes);
	return name == QLatin1String("activity") && uri == NS_ACTIVITY;
}

void ActivityFactory::handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(uri);
	Q_UNUSED(attributes);
	m_depth++;
	if(m_depth == 1) {
		m_state = AtNowhere;
		clear();
	} else if (m_depth == 2) {
		if(name == QLatin1String("text")) {
			m_state = AtText;
		} else {
			m_general = generalByName(name);
			m_state = AtType;
		}
	} else if (m_depth == 3 && m_state == AtType) {
		m_specific = specificByName(name);
	}
}

void ActivityFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	Q_UNUSED(name);
	Q_UNUSED(uri);
	if (m_depth == 2)
		m_state = AtNowhere;
	m_depth--;
}

void ActivityFactory::handleCharacterData(const QStringRef &text)
{
	if (m_depth == 2 && m_state == AtText)
		m_text = text.toString();
}

void ActivityFactory::serialize(Payload *extension, QXmlStreamWriter *writer)
{
	Activity *activity = se_cast<Activity*>(extension);
	if(activity->general() == Activity::InvalidGeneral)
		return;

	writer->writeStartElement(QLatin1String("activity"));
	writer->writeDefaultNamespace(NS_ACTIVITY);
	if (activity->general() != Activity::EmptyGeneral) {
		writer->writeStartElement(enumToStr(activity->general(),general_types));
		if (activity->specific() > Activity::InvalidSpecific)
			writer->writeEmptyElement(enumToStr(activity->specific(),specific_types));
		writer->writeEndElement();
		if(!activity->text().isEmpty())
			writeTextElement(writer,QLatin1String("text"),activity->text());
	}
	writer->writeEndElement();
}

void ActivityFactory::clear()
{
	m_general = Activity::InvalidGeneral;
	m_specific = Activity::InvalidSpecific;
	m_text.clear();
}

Payload::Ptr ActivityFactory::createPayload()
{
	Activity *activity = new Activity(m_general,m_specific,m_text);
	clear();
	return Payload::Ptr(activity);
}

template <typename T>
static T typeByName(const QStringRef &name, const char* types[], int n)
{
	if (name.isEmpty())
		return static_cast<T>(-2); // Empty
	ActivityLessThen activityLessThen;
	const char **res = qBinaryFind(types, types + n, name, activityLessThen);
	return static_cast<T>((res == types + n) ? -1 : (res - types));
}

QLatin1String ActivityFactory::generalName(Activity::General general)
{
	return QLatin1String(general <= Activity::InvalidGeneral ? 0 : general_types[general]);
}

Activity::General ActivityFactory::generalByName(const QStringRef &general)
{
	return typeByName<Activity::General>(general, general_types, sizeof(general_types)/sizeof(char*));
}

QLatin1String ActivityFactory::specificName(Activity::Specific specific)
{
	return QLatin1String(specific <= Activity::InvalidSpecific ? 0 : specific_types[specific]);
}

Activity::Specific ActivityFactory::specificByName(const QStringRef &specific)
{
	return typeByName<Activity::Specific>(specific, specific_types, sizeof(specific_types)/sizeof(char*));
}

} // namespace Jreen
