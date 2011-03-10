/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#ifndef ZLIBDATASTREAM_H
#define ZLIBDATASTREAM_H

#include "datastream.h"

namespace Jreen
{
	class ZLibDataStreamPrivate;
	class ZLibDataStream : public DataStream
	{
		Q_OBJECT
		Q_DECLARE_PRIVATE(ZLibDataStream)
	public:
		ZLibDataStream();
		~ZLibDataStream();
		
		qint64 bytesAvailable() const;
		bool open(OpenMode mode);
		void close();
	protected:
		void incomingDataReady();
		qint64 writeData(const char *data, qint64 len);
		qint64 readData(char *data, qint64 maxlen);
	private:
		QScopedPointer<ZLibDataStreamPrivate> d_ptr;
	};
}

#endif // ZLIBDATASTREAM_H
