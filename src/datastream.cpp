/****************************************************************************
 *  datastream.cpp
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

#include "datastream.h"

namespace Jreen
{
	
	DataStream::DataStream() : m_device(0)
	{
	}
	
	DataStream::~DataStream()
	{
	}
	
	void DataStream::setDevice(QIODevice *device)
	{
		if (m_device)
			disconnect(m_device, 0, this, 0);
		m_device = device;
		connect(m_device, SIGNAL(readyRead()), SLOT(incomingDataReady()));
	}
	
	QIODevice *DataStream::device()
	{
		return m_device;
	}
	
}
