/****************************************************************************
 *  activity.h
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

#ifndef ACTIVITY_H
#define ACTIVITY_H
#include "stanzaextension.h"

namespace jreen {

//XEP-108
//http://xmpp.org/extensions/xep-0108.html
class Activity : public StanzaExtension
{
	J_EXTENSION(jreen::Activity,"/message/activity[@xmlns='http://jabber.org/protocol/activity']")
public:
	enum General {
		DoingChores = 0,
		Drinking,
		Eating,
		Exercising,
		Grooming,
		HavingAppointment,
		Inactive,
		Relaxing,
		Talking,
		Traveling,
		Working
	};
	enum DoingChores {
		BuyingGroceries,
		Cleaning,
		Cooking,
		DoingMaintenance,
		DoingTheDishes,
		DoingTheLaundry,
		Gardening,
		RunningAnErrand,
		WalkingTheDog
	};
	Activity(General general,int specific = -1,const QString &text = QString())
		:	m_general(general),m_specific(specific),m_text(text) {}
	General general() const {return m_general;}
	int specific() const {return m_specific;}
	template<typename T>
	T specific() const {return static_cast<T>(m_specific);}
	const QString &text() const {return m_text;}
	void setText(const QString &text) {m_text = text;}
private:
	General m_general;
	int m_specific;
	QString m_text;
};

} // namespace jreen

#endif // ACTIVITY_H
