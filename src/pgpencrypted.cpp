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

#include "pgpencrypted.h"

namespace Jreen
{

class PGPEncryptedPrivate
{
public:
	QString encryptedText;
};

PGPEncrypted::PGPEncrypted(const QString &encryptedText) : d_ptr(new PGPEncryptedPrivate)
{
	d_func()->encryptedText = encryptedText;
}

PGPEncrypted::~PGPEncrypted()
{
}

QString PGPEncrypted::encryptedText() const
{
	return d_func()->encryptedText;
}

void PGPEncrypted::setEncryptedText(const QString &encryptedText)
{
	d_func()->encryptedText = encryptedText;
}

} // namespace Jreen
