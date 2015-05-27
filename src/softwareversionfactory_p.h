/****************************************************************************
**
** Jreen
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#ifndef JREEN_SOFTWAREVERSIONFACTORY_P_H
#define JREEN_SOFTWAREVERSIONFACTORY_P_H
#include "softwareversion.h"

namespace Jreen {

class JREEN_AUTOTEST_EXPORT SoftwareVersionFactoryPrivate;
class JREEN_AUTOTEST_EXPORT SoftwareVersionFactory : public PayloadFactory<SoftwareVersion>
{
	Q_DECLARE_PRIVATE(SoftwareVersionFactory)
public:
	SoftwareVersionFactory();
	virtual ~SoftwareVersionFactory();
	QStringList features() const;
	bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleEndElement(const QStringRef &name, const QStringRef &uri);
	void handleCharacterData(const QStringRef &text);
	void serialize(Payload *extension, QXmlStreamWriter *writer);
	Payload::Ptr createPayload();
private:
	QScopedPointer<SoftwareVersionFactoryPrivate> d_ptr;
};

} // namespace Jreen

#endif // JREEN_SOFTWAREVERSIONFACTORY_P_H
