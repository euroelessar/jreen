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

#include "zlibdatastream_p.h"
#include <zlib.h>
#include <QBasicTimer>
#include <QTimerEvent>

namespace Jreen
{
class ZLibDataStreamPrivate
{
public:
	z_stream zinflate;
	z_stream zdeflate;
	QByteArray buffer;
	int offset;
	int len;
	void ensureSize(int size)
	{
		if (buffer.size() - offset - len < size)
			buffer.resize(offset + len + size);
	}
};

ZLibDataStream::ZLibDataStream() : d_ptr(new ZLibDataStreamPrivate)
{
	Q_D(ZLibDataStream);
	memset(&d->zinflate, 0, sizeof(z_stream));
	memset(&d->zdeflate, 0, sizeof(z_stream));
	d->offset = 0;
	d->len = 0;
}

ZLibDataStream::~ZLibDataStream()
{
	close();
}

qint64 ZLibDataStream::bytesAvailable() const
{
	return d_func()->len + QIODevice::bytesAvailable();
}

bool ZLibDataStream::open(OpenMode mode)
{
	Q_D(ZLibDataStream);
	memset(&d->zinflate, 0, sizeof(z_stream));
	memset(&d->zdeflate, 0, sizeof(z_stream));
	int res = inflateInit(&d->zinflate);
	if (res != Z_OK)
		return false;
	res = deflateInit(&d->zdeflate, Z_BEST_COMPRESSION);
	if (res != Z_OK) {
		inflateEnd(&d->zinflate);
		return false;
	}
	QIODevice::open(mode);
	return true;
}

void ZLibDataStream::close()
{
	Q_D(ZLibDataStream);
	if (!isOpen())
		return;
	inflateEnd(&d->zinflate);
	deflateEnd(&d->zdeflate);
}

void ZLibDataStream::incomingDataReady()
{
	Q_D(ZLibDataStream);
	QByteArray data = device()->readAll();
	const char chunkSize = 100;
	d->zinflate.next_in = reinterpret_cast<Bytef*>(data.data());
	d->zinflate.avail_in = data.size();
	do {
		d->ensureSize(chunkSize);
		d->zinflate.avail_out = chunkSize;
		d->zinflate.next_out = reinterpret_cast<Bytef*>(d->buffer.data() + d->offset + d->len);
		inflate(&d->zinflate, Z_SYNC_FLUSH);
		d->len += chunkSize - d->zinflate.avail_out;
	} while (d->zinflate.avail_out == 0);
	emit readyRead();
}

qint64 ZLibDataStream::writeData(const char *data, qint64 len)
{
	Q_D(ZLibDataStream);
	if (len <= 0)
		return 0;
	d->zdeflate.avail_in = len;
	d->zdeflate.next_in = reinterpret_cast<Bytef*>(const_cast<char *>(data));
	const int chunkSize = d->zdeflate.avail_in * 1.01 + 13;
	QByteArray buffer(chunkSize, Qt::Uninitialized);
	do {
		d->zdeflate.avail_out = buffer.size();
		d->zdeflate.next_out = reinterpret_cast<Bytef*>(buffer.data());
		deflate(&d->zdeflate, Z_SYNC_FLUSH);
		device()->write(buffer.data(), chunkSize - d->zdeflate.avail_out);
	} while (d->zdeflate.avail_out == 0);
	return len;
}

qint64 ZLibDataStream::readData(char *data, qint64 maxlen)
{
	Q_D(ZLibDataStream);
	int len = qMin<int>(maxlen, d->len);
	qMemCopy(data, d->buffer.data() + d->offset, len);
	if (maxlen < d->len) {
		d->len -= maxlen;
		d->offset += maxlen;
	} else {
		d->offset = 0;
		d->len = 0;
	}
	return len;
}
}
