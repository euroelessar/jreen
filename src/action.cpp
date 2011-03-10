/****************************************************************************
 *  action.cpp
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

#include "action.h"

namespace Jreen
{

Action::Action(QObject *parent) : QObject(parent)
{
}

void Action::trigger()
{
	emit triggered();
}

void Action::setData(const QVariant &data, int role)
{
	m_data.insert(role, data);
}

QVariant Action::data(int role)
{
	return m_data.value(role, QVariant());
}

}
