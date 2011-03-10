/****************************************************************************
 *  action.h
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

#ifndef ACTION_H
#define ACTION_H

#include <QObject>
#include <QMap>
#include <QVariant>
#include "jreen.h"

namespace Jreen
{

class JREEN_EXPORT Action : public QObject
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
