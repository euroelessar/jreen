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

#include <QObject>
#include "jreen.h"

namespace jreen
{

class JREEN_EXPORT DataStream : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(DataStream)
public:
	DataStream();
	virtual ~DataStream();
public slots:
	virtual void write( const QByteArray &data );
	virtual void writeIncoming( const QByteArray &data );
signals:
	void read( const QByteArray &data );
	void readOutgoing( const QByteArray &data );
};

}

#endif // DATASTREAM_H
