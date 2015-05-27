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

#ifndef JREEN_PGPENCRYPTED_H
#define JREEN_PGPENCRYPTED_H

#include "stanzaextension.h"

namespace Jreen
{

class PGPEncryptedPrivate;

class JREEN_EXPORT PGPEncrypted : public Payload
{
	J_PAYLOAD(Jreen::PGPEncrypted)
	Q_DECLARE_PRIVATE(PGPEncrypted)
public:
	PGPEncrypted(const QString &encryptedText = QString());
	~PGPEncrypted();

	QString encryptedText() const;
	void setEncryptedText(const QString &encryptedText);

private:
	QScopedPointer<PGPEncryptedPrivate> d_ptr;
};
} // namespace Jreen

#endif // JREEN_PGPENCRYPTED_H
