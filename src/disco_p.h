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

#ifndef DISCO_P_H
#define DISCO_P_H

#include "disco.h"
#include "dataformfactory_p.h"

namespace Jreen
{
class DiscoPrivate
{
public:
	Disco::IdentityList identities;
	QSet<QString> features;
	Client *client;
	QSharedPointer<DataForm> form;
	QString os;
	QString software_name;
	QString software_version;
	
	void _q_received(const Jreen::IQ &iq);
	static DiscoPrivate *get(Disco *disco) { return disco->d_func(); }
};

class JREEN_AUTOTEST_EXPORT DiscoInfoFactory : public PayloadFactory<Disco::Info>
{
public:
	DiscoInfoFactory();
	QStringList features() const;
	bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleEndElement(const QStringRef &name, const QStringRef &uri);
	void handleCharacterData(const QStringRef &text);
	void serialize(Payload *extension, QXmlStreamWriter *writer);
	Payload::Ptr createPayload();
private:
	enum State { AtStart, AtInfo, AtDataForm };
	int m_depth;
	State m_state;
	QString m_node;
	Disco::IdentityList m_identities;
	QSet<QString> m_features;
	DataFormFactory m_factory;
	bool m_hasDataForm;
};

class JREEN_AUTOTEST_EXPORT DiscoItemsFactory : public PayloadFactory<Disco::Items>
{
public:
	DiscoItemsFactory();
	virtual ~DiscoItemsFactory() {}
	QStringList features() const;
	bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleEndElement(const QStringRef &name, const QStringRef &uri);
	void handleCharacterData(const QStringRef &text);
	void serialize(Payload *extension, QXmlStreamWriter *writer);
	Payload::Ptr createPayload();
private:
	int m_depth;
	QString m_node;
	Disco::ItemList m_items;
};

}

#endif // DISCO_P_H
