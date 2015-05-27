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

#ifndef JREEN_DATASTREAM_H
#define JREEN_DATASTREAM_H

#include <QIODevice>
#include "jreen.h"

namespace Jreen
{

class JREEN_EXPORT DataStream : public QIODevice
{
	Q_OBJECT
	Q_DISABLE_COPY(DataStream)
public:
	DataStream();
	virtual ~DataStream();
	void setDevice(QIODevice *device);
	QIODevice *device();
protected slots:
	virtual void incomingDataReady() = 0;
private:
	QIODevice *m_device;
};

}

#endif // JREEN_DATASTREAM_H
