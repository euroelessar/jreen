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
