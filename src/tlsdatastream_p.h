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

#ifndef TLSDATASTREAM_H
#define TLSDATASTREAM_H

#include "datastream.h"
#include <QtCrypto>

namespace Jreen
{
	class TLSDataStream : public DataStream
	{
		Q_OBJECT
	public:
		TLSDataStream(QCA::TLS *tls);
		~TLSDataStream();
		
		qint64 bytesAvailable() const;
		bool open(OpenMode mode);
		void close();
	protected:
		void incomingDataReady();
		qint64 writeData(const char *data, qint64 len);
		qint64 readData(char *data, qint64 maxlen);
	private slots:
		void onReadyRead();
		void onReadyReadOutgoing();
	private:
		QWeakPointer<QCA::TLS> m_tls;
		QByteArray m_buffer;
		int m_offset;
	};
}

#endif // TLSDATASTREAM_H
