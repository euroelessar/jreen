/****************************************************************************
 *  datastream.h
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

#ifndef DATASTREAM_H
#define DATASTREAM_H

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

#endif // DATASTREAM_H
