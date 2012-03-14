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

#include "mood.h"
#include "moodfactory_p.h"

namespace Jreen
{
class MoodPrivate
{
public:
	Mood::Type type;
	QString text;
};

Mood::Mood(Type type, const QString &text) : d_ptr(new MoodPrivate)
{
	Q_D(Mood);
	d->type = type;
	d->text = text;
}

Mood::Mood(const QString &type, const QString &text) : d_ptr(new MoodPrivate)
{
	Q_D(Mood);
	setType(type);
	d->text = text;
}

Mood::~Mood()
{
}

Mood::Type Mood::type() const
{
	return d_func()->type;
}

QString Mood::typeName() const
{
	return MoodFactory::typeName(d_func()->type);
}

void Mood::setType(const QString &type)
{
	d_func()->type = MoodFactory::typeByName(QStringRef(&type));
}

void Mood::setType(Type type)
{
	d_func()->type = type;
}

QString Mood::text() const
{
	return d_func()->text;
}

void Mood::setText(const QString &text)
{
	d_func()->text = text;
}
} // namespace Jreen
