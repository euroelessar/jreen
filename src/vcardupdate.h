/****************************************************************************
**
** Jreen
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#ifndef JREEN_VCARDUPDATE_H
#define JREEN_VCARDUPDATE_H
#include "stanzaextension.h"

namespace Jreen
{

class VCardUpdatePrivate;

//XEP-0153: vCard-Based Avatars
//http://xmpp.org/extensions/xep-0153.html
class JREEN_EXPORT VCardUpdate : public Payload
{
	J_PAYLOAD(Jreen::VCardUpdate)
	Q_DECLARE_PRIVATE(VCardUpdate)
public:
	VCardUpdate(const QString &photoHash);
	VCardUpdate();
	~VCardUpdate();

	QString photoHash() const;
	void setPhotoHash(const QString &photoHash);
	void clear();
	bool hasPhotoInfo() const;

private:
	QScopedPointer<VCardUpdatePrivate> d_ptr;
};

} // namespace Jreen

#endif // JREEN_VCARDUPDATE_H
