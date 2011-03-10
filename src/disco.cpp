/****************************************************************************
 *  disco.cpp
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#include "disco_p.h"
#include "iq.h"
#include "jstrings.h"
#include "dataform.h"
#include "softwareversion.h"
#include "client.h"
#include <QXmlStreamWriter>

#define NS_DISCO_INFO QLatin1String("http://jabber.org/protocol/disco#info")
#define NS_DISCO_ITEMS QLatin1String("http://jabber.org/protocol/disco#items")

namespace Jreen
{

DiscoInfoFactory::DiscoInfoFactory()
{
	m_depth = 0;
	m_state = AtStart;
	m_hasDataForm = false;
}

QStringList DiscoInfoFactory::features() const
{
	return QStringList(NS_DISCO_INFO);
}

bool DiscoInfoFactory::canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(attributes);
	return name == QLatin1String("query") && uri == NS_DISCO_INFO;
}

void DiscoInfoFactory::handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	m_depth++;
	if (m_depth == 1) {
		m_node = attributes.value(QLatin1String("node")).toString();
		m_identities.clear();
		m_features.clear();
		m_hasDataForm = false;
		m_state = AtInfo;
	} else if (m_depth == 2) {
		if (name == QLatin1String("identity")) {
			Disco::Identity identity(attributes.value(QLatin1String("category")).toString(),
									 attributes.value(QLatin1String("type")).toString(),
									 attributes.value(QLatin1String("name")).toString(),
									 attributes.value(QLatin1String("lang")).toString());
			m_identities.append(identity);
		} else if (name == QLatin1String("feature")) {
			m_features.insert(attributes.value(QLatin1String("var")).toString());
		} else if (m_factory.canParse(name, uri, attributes)) {
			m_factory.handleStartElement(name, uri, attributes);
			m_hasDataForm = true;
			m_state = AtDataForm;
		}
	}
}

void DiscoInfoFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	if (m_state == AtDataForm)
		m_factory.handleEndElement(name, uri);
	if (m_depth == 2 && m_state == AtDataForm) {
		m_state = AtInfo;
	} else if (m_depth == 1) {
		m_state = AtStart;
	}
	m_depth--;
}

void DiscoInfoFactory::handleCharacterData(const QStringRef &text)
{
	if (m_state == AtDataForm)
		m_factory.handleCharacterData(text);
}

void DiscoInfoFactory::serialize(StanzaExtension *extension, QXmlStreamWriter *writer)
{
	Disco::Info *info = se_cast<Disco::Info*>(extension);
	if (!info)
		return;
	writer->writeStartElement(QLatin1String("query"));
	if (!info->node().isEmpty())
		writer->writeAttribute(QLatin1String("node"),info->node());
	writer->writeDefaultNamespace(NS_DISCO_INFO);
	foreach (const Disco::Identity &identity, info->identities()) {
		writer->writeEmptyElement(QLatin1String("identity"));
		writer->writeAttribute(QLatin1String("category"), identity.category);
		writer->writeAttribute(QLatin1String("type"), identity.type);
		writer->writeAttribute(QLatin1String("name"), identity.name);
		if (!identity.lang.isEmpty())
			writer->writeAttribute(QLatin1String("lang"), identity.lang);
	}
	foreach (const QString &feature, info->features()) {
		writer->writeEmptyElement(QLatin1String("feature"));
		writer->writeAttribute(QLatin1String("var"), feature);
	}
	if (info->form())
		m_factory.serialize(info->form().data(), writer);
	writer->writeEndElement();
}

StanzaExtension::Ptr DiscoInfoFactory::createExtension()
{
	StanzaExtension::Ptr dataForm;
	if (m_hasDataForm)
		dataForm = m_factory.createExtension();
	return StanzaExtension::Ptr(new Disco::Info(m_node, m_identities,
												m_features, dataForm.staticCast<DataForm>()));
}

DiscoItemsFactory::DiscoItemsFactory()
{

}

QStringList DiscoItemsFactory::features() const
{
	return QStringList(NS_DISCO_ITEMS);
}

bool DiscoItemsFactory::canParse(const QStringRef &name, const QStringRef &uri,
			  const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(attributes);
	return name == QLatin1String("query") && uri == NS_DISCO_ITEMS;
}

void DiscoItemsFactory::handleStartElement(const QStringRef &name,
						const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(name);
	Q_UNUSED(uri);
	m_depth++;
	if(m_depth == 1) {
		m_items.clear();
		m_node = attributes.value("node").toString();
	} else if(m_depth == 2) {
		Disco::Item item;
		item.jid = attributes.value("jid").toString();
		item.name = attributes.value("name").toString();
		item.node = attributes.value("node").toString();
		m_items.append(item);
	}
}

void DiscoItemsFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	Q_UNUSED(name);
	Q_UNUSED(uri);
	m_depth--;
}

void DiscoItemsFactory::handleCharacterData(const QStringRef &text)
{
	Q_UNUSED(text);
}

void DiscoItemsFactory::serialize(StanzaExtension *extension, QXmlStreamWriter *writer)
{
	Disco::Items *items = se_cast<Disco::Items*>(extension);
	writer->writeStartElement(QLatin1String("query"));
	writer->writeDefaultNamespace(NS_DISCO_ITEMS);
	if (!items->node().isEmpty())
		writer->writeAttribute(QLatin1String("node"), items->node());
	foreach(const Disco::Item &item,items->items()) {
		writer->writeEmptyElement(QLatin1String("item"));
		writer->writeAttribute(QLatin1String("jid"),item.jid);
		if (!item.node.isEmpty())
			writer->writeAttribute(QLatin1String("node"),item.node);
		writer->writeAttribute(QLatin1String("name"),item.name);
	}
	writer->writeEndElement();
}

StanzaExtension::Ptr DiscoItemsFactory::createExtension()
{
	return StanzaExtension::Ptr(new Disco::Items(m_node,m_items));
}

Disco::Disco(Client *client) : d_ptr(new DiscoPrivate)
{
	Q_D(Disco);
	d->client = client;
	connect(d->client, SIGNAL(newIQ(Jreen::IQ)), this, SLOT(handleIQ(Jreen::IQ)));
}

Disco::~Disco()
{
}

const Disco::IdentityList &Disco::identities() const
{
	Q_D(const Disco);
	return d->identities;
}

Disco::IdentityList &Disco::identities()
{
	Q_D(Disco);
	return d->identities;
}

void Disco::addIdentity(const Identity &identity)
{
	d_func()->identities.append(identity);
}

const QSet<QString> &Disco::features() const
{
	Q_D(const Disco);
	return d->features;
}

void Disco::addFeature(const QString &feature)
{
	d_func()->features << feature;
}

QSet<QString> &Disco::features()
{
	Q_D(Disco);
	return d->features;
}

void Disco::handleIQ(const Jreen::IQ &iq)
{
	Q_D(Disco);
	const Info *info = iq.findExtension<Info>().data();
	if(info) {
		if(iq.subtype() == IQ::Get)	{
			IQ receipt(IQ::Result, iq.from(), iq.id());
			receipt.addExtension(new Info(info->node(), d->identities, d->features, d->form));
			d->client->send(receipt);
			iq.accept();
		}
	}
	const Items *items = iq.findExtension<Items>().data();
	if(items) {
		if(iq.subtype() == IQ::Get)	{
			IQ receipt(IQ::Result, iq.from(), iq.id());
			receipt.addExtension(new Items(items->node()));
			d->client->send(receipt);
			iq.accept();
		}
	}
	const SoftwareVersion *version = iq.findExtension<SoftwareVersion>().data();
	if(version)	{
		if(iq.subtype() == IQ::Get)	{
			IQ receipt(IQ::Result, iq.from(), iq.id());
			receipt.addExtension(new SoftwareVersion(d->software_name, d->software_version, d->os));
			d->client->send(receipt);
			iq.accept();
		}
	}
}

void Disco::setSoftwareVersion(const QString &name, const QString &version, const QString &os)
{
	Q_D(Disco);
	d->software_name = name;
	d->software_version = version;
	d->os = os;
	QSharedPointer<DataForm> form(new DataForm(DataForm::Submit,QString()));
	form->appendField(DataFormFieldHidden(QLatin1String("FORM_TYPE"), QLatin1String("urn:xmpp:dataforms:softwareinfo")));
	form->appendField(DataFormFieldNone(QLatin1String("ip_version"), QStringList() << QLatin1String("ipv4") << QLatin1String("ipv6")));
	form->appendField(DataFormFieldNone(QLatin1String("os"), QStringList(os)));
	form->appendField(DataFormFieldNone(QLatin1String("software"), QStringList(name)));
	form->appendField(DataFormFieldNone(QLatin1String("software_version"), QStringList(version)));
	d->form = form;
}

const DataForm *Disco::form() const
{
	Q_D(const Disco);
	return d->form.data();
}

void Disco::setForm(DataForm *form)
{
	Q_D(Disco);
	d->form = QSharedPointer<DataForm>(form);
}

}
