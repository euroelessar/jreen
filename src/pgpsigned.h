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

#ifndef JREEN_PGPSIGNED_H
#define JREEN_PGPSIGNED_H

#include "stanzaextension.h"

namespace Jreen
{

class PGPSignedPrivate;

class JREEN_EXPORT PGPSigned : public Payload
{
	J_PAYLOAD(Jreen::PGPSigned)
	Q_DECLARE_PRIVATE(PGPSigned)
public:
	PGPSigned(const QString &signature = QString());
	~PGPSigned();

	QString signature() const;
	void setSignature(const QString &signature);

private:
	QScopedPointer<PGPSignedPrivate> d_ptr;
};

} // namespace Jreen

#endif // JREEN_PGPSIGNED_H
