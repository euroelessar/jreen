/****************************************************************************
 *  activity.h
 *
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
 *  Copyright (c) 2011 by Prokhin Alexey <alexey.prokhin@yandex.ru>
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

namespace Jreen {

//XEP-108
//http://xmpp.org/extensions/xep-0108.html

class ActivityPrivate;
class JREEN_EXPORT Activity : public StanzaExtension
{
	Q_DECLARE_PRIVATE(Activity);
	J_EXTENSION(Jreen::Activity,"/message/activity[@xmlns='http://jabber.org/protocol/activity']")
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
		Undefined,
		Working,
		InvalidGeneral = -1,
		EmptyGeneral = -2
	};
	enum Specific {
		AtTheSpa,
		BrushingTeeth,
		BuyingGroceries,
		Cleaning,
		Coding,
		Commuting,
		Cooking,
		Cycling,
		Dancing,
		DayOff,
		DoingMaintenance,
		DoingTheDishes,
		DoingTheLaundry,
		Driving,
		Fishing,
		Gaming,
		Gardening,
		GettingAHaircut,
		GoingOut,
		HangingOut,
		HavingABeer,
		HavingASnack,
		HavingBreakfast,
		HavingCoffee,
		HavingDinner,
		HavingLunch,
		HavingTea,
		Hiding,
		Hiking,
		InACar,
		InAMeeting,
		InRealLife,
		Jogging,
		OnABus,
		OnAPlane,
		OnATrain,
		OnATrip,
		OnThePhone,
		OnVacation,
		OnVideoPhone,
		Other,
		Partying,
		PlayingSports,
		Praying,
		Reading,
		Rehearsing,
		Running,
		RunningAnErrand,
		ScheduledHoliday,
		Shaving,
		Shopping,
		Skiing,
		Sleeping,
		Smoking,
		Socializing,
		Studying,
		Sunbathing,
		Swimming,
		TakingABath,
		TakingAShower,
		Thinking,
		Walking,
		WalkingTheDog,
		WatchingAMovie,
		WatchingTv,
		WorkingOut,
		Writing,

		InvalidSpecific = -1,
		EmptySpecific = -2
	};

	Activity(General general, Specific specific = InvalidSpecific, const QString &text = QString());
	Activity(const QString &general, const QString &specific = QString(), const QString &text = QString());
	virtual ~Activity();

	General general() const;
	QString generalName() const;
	static QString generalName(General general);

	Specific specific() const;
	QString specificName();
	static QString specificName(Specific specific);

	const QString &text() const;
	void setText(const QString &text);
private:
	QScopedPointer<ActivityPrivate> d_ptr;
};

} // namespace Jreen

#endif // ACTIVITY_H
