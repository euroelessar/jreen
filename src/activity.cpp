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

#include "activity.h"
#include "activityfactory_p.h"

namespace Jreen {

class ActivityPrivate
{
public:
	Activity::General general;
	Activity::Specific specific;
	QString text;
};

Activity::Activity(General general, Specific specific, const QString &text) :
	d_ptr(new ActivityPrivate)
{
	Q_D(Activity);
	d->general = general;
	d->specific = specific;
	d->text = text;
}

Activity::Activity(const QString &general, const QString &specific, const QString &text) :
	d_ptr(new ActivityPrivate)
{
	Q_D(Activity);
	d->general = ActivityFactory::generalByName(QStringRef(&general));
	d->specific = ActivityFactory::specificByName(QStringRef(&specific));
	d->text = text;
}

Activity::~Activity()
{

}

Activity::General Activity::general() const
{
	return d_func()->general;
}

QString Activity::generalName() const
{
	return ActivityFactory::generalName(d_func()->general);
}

QString Activity::generalName(General general)
{
	return ActivityFactory::generalName(general);
}

Activity::Specific Activity::specific() const
{
	return d_func()->specific;
}

QString Activity::specificName()
{
	return ActivityFactory::specificName(d_func()->specific);
}

QString Activity::specificName(Specific specific)
{
	return ActivityFactory::specificName(specific);
}

const QString &Activity::text() const
{
	return d_func()->text;
}

void Activity::setText(const QString &text)
{
	d_func()->text = text;
}

} // namespace Jreen
