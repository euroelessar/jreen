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
** Jreen is free software: you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** Jreen is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with Jreen.  If not, see <http://www.gnu.org/licenses/>.
** $JREEN_END_LICENSE$
**
****************************************************************************/

#ifndef JREEN_ACTIVITY_H
#define JREEN_ACTIVITY_H
#include "stanzaextension.h"

namespace Jreen {

//XEP-108
//http://xmpp.org/extensions/xep-0108.html

class ActivityPrivate;
class JREEN_EXPORT Activity : public Payload
{
	Q_DECLARE_PRIVATE(Activity)
	J_PAYLOAD(Jreen::Activity)
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

#endif // JREEN_ACTIVITY_H
