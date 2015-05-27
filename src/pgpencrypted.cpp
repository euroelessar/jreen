/****************************************************************************
**
** Jreen
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $JREEN_BEGIN_LICENSE$
** Jreen is free software: you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** Jreen is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with Jreen.  If not, see <http://www.gnu.org/licenses/>.
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
