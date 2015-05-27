/****************************************************************************
**
** Jreen
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef JREEN_ACTION_P_H
#define JREEN_ACTION_P_H

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

#endif // JREEN_ACTION_P_H
