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

#include "tlsdatastream.h"
#include <QDebug>

namespace jreen
{
	TLSDataStream::TLSDataStream(QCA::TLS *tls) : m_tls(tls)
	{
		connect(m_tls, SIGNAL(readyRead()), SLOT(onReadyRead()));
		connect(m_tls, SIGNAL(readyReadOutgoing()), SLOT(onReadyReadOutgoing()));
		m_offset = 0;
	}
	
	TLSDataStream::~TLSDataStream()
	{
	}
	
	qint64 TLSDataStream::bytesAvailable() const
	{
		return m_buffer.size() - m_offset;
	}
	
	bool TLSDataStream::open(OpenMode mode)
	{
		return QIODevice::open(mode);
	}

	void TLSDataStream::close()
	{
	}
	
	void TLSDataStream::incomingDataReady()
	{
		m_tls->writeIncoming(device()->readAll());
	}

	qint64 TLSDataStream::writeData(const char *data, qint64 len)
	{
		m_tls->write(QByteArray(data, len));
		return len;
	}

	qint64 TLSDataStream::readData(char *data, qint64 maxlen)
	{
		if (maxlen >= m_buffer.size() - m_offset) {
			int len = m_buffer.size() - m_offset;
			qMemCopy(data, m_buffer.constData() + m_offset, len);
			m_buffer.clear();
			m_offset = 0;
			return len;
		}
		qMemCopy(data, m_buffer.constData() + m_offset, maxlen);
		m_offset += maxlen;
		return maxlen;
	}
	
	void TLSDataStream::onReadyRead()
	{
		m_buffer.append(m_tls->read());
		emit readyRead();
	}

	void TLSDataStream::onReadyReadOutgoing()
	{
		device()->write(m_tls->readOutgoing());
	}
}
