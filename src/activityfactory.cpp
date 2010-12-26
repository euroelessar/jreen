/****************************************************************************
 *  activityfactory.cpp
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
#include <QXmlStreamWriter>
#include <qstringlist.h>
#include "activityfactory_p.h"
#include "jstrings.h"
#include "util.h"
#define NS_ACTIVITY QLatin1String("http://jabber.org/protocol/activity")

namespace jreen {

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

static const char* doing_chores_types[]= {
	"buying_groceries",
	"cleaning",
	"cooking",
	"doing_maintenance",
	"doing_the_dishes",
	"doing_the_laundry",
	"gardening",
	"running_an_errand",
	"walking_the_dog"
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
		if(name == QLatin1String("text")) {
			m_state = AtText;
		} else {
			m_general = strToEnum<Activity::General>(name,general_types);
			m_state = AtType;
		}
	} if(m_depth == 2) {
		switch(m_general) {
		case Activity::DoingChores:
			m_specific = strToEnum(name,doing_chores_types);
			break;
		}
	}
}

void ActivityFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	Q_UNUSED(name);
	Q_UNUSED(uri);
	m_depth--;
}

void ActivityFactory::handleCharacterData(const QStringRef &text)
{
	if(m_state == AtText)
		m_text = text.toString();
}

void ActivityFactory::serialize(StanzaExtension *extension, QXmlStreamWriter *writer)
{
	Activity *activity = se_cast<Activity*>(extension);

	writer->writeStartElement(QLatin1String("activity"));
	writer->writeDefaultNamespace(NS_ACTIVITY);
	writer->writeStartElement(enumToStr(activity->general(),general_types));
	writer->writeEndElement();
	writeTextElement(writer,QLatin1String("text"),activity->text());
	writer->writeEndElement();
}

void ActivityFactory::clear()
{
	m_general = Activity::Invalid;
	m_specific = -1;
	m_text.clear();
}

StanzaExtension::Ptr ActivityFactory::createExtension()
{
	Activity *activity = new Activity(m_general,m_specific,m_text);
	clear();
	return StanzaExtension::Ptr(activity);
}

} // namespace jreen
