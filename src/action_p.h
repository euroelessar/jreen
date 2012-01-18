/****************************************************************************
**
** Jreen
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef ACTION_H
#define ACTION_H

#include <QObject>
#include <QMap>
#include <QVariant>
#include "jreen.h"

namespace Jreen
{

class Action : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(Action)
public:
	Action(QObject *parent);
	void trigger();
	void setData(const QVariant &data, int role = 0);
	QVariant data(int role = 0);
signals:
	void triggered();
private:
	QMap<int, QVariant> m_data;
};

}

#endif // ACTION_H
