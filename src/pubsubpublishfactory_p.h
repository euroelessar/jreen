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

#ifndef PUBSUBPUBLISHFACTORY_P_H
#define PUBSUBPUBLISHFACTORY_P_H

#include "pubsubmanager_p.h"
#include "dataformfactory_p.h"

namespace Jreen
{
namespace PubSub
{
class JREEN_AUTOTEST_EXPORT PublishFactory : public PayloadFactory<Publish>
{
public:
	PublishFactory(QList<AbstractPayloadFactory*> &factories);
	~PublishFactory();
	QStringList features() const;
	bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleEndElement(const QStringRef &name, const QStringRef &uri);
	void handleCharacterData(const QStringRef &text);
	void serialize(Payload *extension, QXmlStreamWriter *writer);
	Payload::Ptr createPayload();
private:
	AbstractPayloadFactory *findFactory(const QStringRef &node);
	AbstractPayloadFactory *findFactory(int type);

	enum State { AtNowhere, AtPublish, AtItem, AtEntity } m_state;
	QScopedPointer<Publish> m_publish;
	AbstractPayloadFactory *m_factory;
	QList<AbstractPayloadFactory*> &m_factories;
	int m_depth;
	DataFormFactory m_form;
};
}
}

#endif // PUBSUBPUBLISHFACTORY_P_H
