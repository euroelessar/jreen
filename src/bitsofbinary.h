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

#ifndef JREEN_BITSOFBINARY_H
#define JREEN_BITSOFBINARY_H

#include "stanzaextension.h"

namespace Jreen
{

class BitsOfBinaryPrivate;

class JREEN_EXPORT BitsOfBinary : public Jreen::Payload
{
	J_PAYLOAD(Jreen::BitsOfBinary)
	Q_DECLARE_PRIVATE(BitsOfBinary)
public:
	BitsOfBinary(const QByteArray &data = QByteArray(), qint64 age = -1);
	BitsOfBinary(const QUrl &cid);
	~BitsOfBinary();
	
	QUrl cid() const;
	void setCid(const QUrl &cid);
	qint64 maximumAge() const;
	void setMaximumAge(qint64 age);
	QString type() const;
	void setType(const QString &type);
	QByteArray data() const;
	void setData(const QByteArray &data);
	
private:
	QScopedPointer<BitsOfBinaryPrivate> d_ptr;
};

} // namespace Jreen

#endif // JREEN_BITSOFBINARY_H
