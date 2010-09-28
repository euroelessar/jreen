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

namespace jreen
{

DataStream::DataStream()
{
}

DataStream::~DataStream()
{
}

void DataStream::write( const QByteArray &data )
{
	emit readOutgoing( data );
}

void DataStream::writeIncoming( const QByteArray &data )
{
	emit read( data );
}

}
