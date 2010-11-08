/****************************************************************************
 *  presence.cpp
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#include "presence_p.h"
#include "capabilities.h"

namespace jreen
{

J_STRING(show)
J_STRING(status)
J_STRING(presence)
J_STRING(unavailable)
J_STRING(error)
J_STRING(probe)
J_STRING(away)
J_STRING(chat)
J_STRING(dnd)
J_STRING(xa)
J_STRING(priority)

Presence::Presence(Type type, const JID& to, const QString &status, int priority, const QString &xmllang)
	: Stanza(*new PresencePrivate)
{
	Q_D(Presence);
	d->subtype = type;
	d->to = to;
	d->priority = priority;
	d->status[xmllang] = status;
}

Presence::Presence(Type type, const JID& to, const LangMap &status, int priority)
	: Stanza(*new PresencePrivate)
{
	Q_D(Presence);
	d->subtype = type;
	d->to = to;
	d->priority = priority;
	d->status = status;
}

Presence::Presence(PresencePrivate &p) : Stanza(p)
{
}

Presence::Presence(const QDomElement &node) : Stanza(node, new PresencePrivate)
{
	Q_D(Presence);
	d->priority = 0;
	if(node.nodeName() != presence_str)
	{
		d->subtype = Invalid;
		return;
	}
	d->subtype = Available;
	QString type = node.attribute(ConstString::type);
	if(type == unavailable_str)
		d->subtype = Unavailable;
	else if(type == error_str)
		d->subtype = Error;
	else if(type == probe_str)
		d->subtype = Probe;
	forelements(const QDomElement &elem, node)
	{
		QString name = elem.nodeName();
		if(d->subtype == Available && name == show_str)
		{
			QString text = elem.text();
			if(text == away_str)
				d->subtype = Away;
			else if(text == chat_str)
				d->subtype = Chat;
			else if(text == dnd_str)
				d->subtype = DND;
			else if(text == xa_str)
				d->subtype = XA;
		}
		else if(name == status_str)
		{
			QString lang = elem.attribute(ConstString::lang);
			d->status[lang] = elem.text();
		}
		else if(name == priority_str)
		{
			d->priority = elem.text().toInt();
		}
	}
}

Presence::Type Presence::subtype() const
{
	Q_D(const Presence);
	return d->subtype;
}

const Capabilities *Presence::capabilities() const
{
	return findExtension<Capabilities>().data();
}

Presence::Type Presence::presence() const
{
	Q_D(const Presence);
	return d->subtype;
}

void Presence::setPresence(Type type)
{
	Q_D(Presence);
	d->subtype = type;
}

const QString &Presence::status(const QString &lang) const
{
	Q_D(const Presence);
	return d->status.value(lang);
}

void Presence::addStatus(const QString &status, const QString &lang)
{
	Q_D(Presence);
	d->status[lang] = status;
}

void Presence::resetStatus()
{
	Q_D(Presence);
	d->status.clear();
	d->status.clear();
}

int Presence::priority() const
{
	Q_D(const Presence);
	return d->priority;
}

void Presence::setPriority(int priority)
{
	Q_D(Presence);
	d->priority = priority;
}

void Presence::writeXml(QXmlStreamWriter *writer) const
{
	Q_D(const Presence);
	writer->writeStartElement(presence_str);
	d->setAttributes(writer);
	switch (d->subtype) {
	case Available:
		break;
	case Chat:
		writer->writeTextElement(show_str, chat_str);
		break;
	case Away:
		writer->writeTextElement(show_str, away_str);
		break;
	case DND:
		writer->writeTextElement(show_str, dnd_str);
		break;
	case XA:
		writer->writeTextElement(show_str, xa_str);
		break;
	case Unavailable:
		writer->writeAttribute(ConstString::type, unavailable_str);
		break;
	default:
		writer->writeEndElement();
		return;
	}
	writer->writeTextElement(priority_str, QString::number(d->priority));
	d->status.fillNode(writer, status_str);
	d->addExtensions(writer);
	writer->writeEndElement();
}
}
