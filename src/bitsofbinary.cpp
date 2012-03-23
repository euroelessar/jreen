/****************************************************************************
**
** Jreen
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "bitsofbinary.h"
#include <QCryptographicHash>
#include <QUrl>

namespace Jreen
{

class BitsOfBinaryPrivate
{
public:
	mutable QUrl cid;
	qint64 maximumAge;
	QByteArray data;
	QString type;
};

BitsOfBinary::BitsOfBinary(const QByteArray &data, qint64 age) : d_ptr(new BitsOfBinaryPrivate)
{
	Q_D(BitsOfBinary);
	d->maximumAge = age;
	d->data = data;
}

BitsOfBinary::BitsOfBinary(const QUrl &cid)
{
	Q_D(BitsOfBinary);
	d->maximumAge = -1;
	d->cid = cid;
}

BitsOfBinary::~BitsOfBinary()
{
}

QUrl BitsOfBinary::cid() const
{
	Q_D(const BitsOfBinary);
	if (!d->data.isEmpty() && d->cid.isEmpty()) {
		QString cid = QLatin1String("sha1+");
		QCryptographicHash hash(QCryptographicHash::Sha1);
		hash.addData(d->data);
		cid += hash.result().toHex();
		cid += QLatin1String("@bob.xmpp.org");
		d->cid = QUrl(cid);
	}
	return d->cid;
}

void BitsOfBinary::setCid(const QUrl &cid)
{
	d_func()->cid = cid;
}

qint64 BitsOfBinary::maximumAge() const
{
	return d_func()->maximumAge;
}

void BitsOfBinary::setMaximumAge(qint64 age)
{
	d_func()->maximumAge = age;
}

QString BitsOfBinary::type() const
{
	return d_func()->type;
}

void BitsOfBinary::setType(const QString &type)
{
	d_func()->type = type;
}

QByteArray BitsOfBinary::data() const
{
	return d_func()->data;
}

void BitsOfBinary::setData(const QByteArray &data)
{
	d_func()->data = data;
}

} // namespace Jreen
