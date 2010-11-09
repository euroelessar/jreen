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
#include <QXmlStreamWriter>

namespace jreen
{
	DiscoInfoFactory::DiscoInfoFactory()
	{
		m_depth = 0;
		m_state = AtStart;
		m_hasDataForm = false;
	}
	
	QStringList DiscoInfoFactory::features() const
	{
		return QStringList(QLatin1String("http://jabber.org/protocol/disco#info"));
	}
	
	bool DiscoInfoFactory::canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
	{
		Q_UNUSED(attributes);
		return name == QLatin1String("query") && uri == QLatin1String("http://jabber.org/protocol/disco#info");
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
		writer->writeStartElement(QLatin1String("info"));
		writer->writeDefaultNamespace(QLatin1String("http://jabber.org/protocol/disco#info"));
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
	
	Disco::Info::Info(const QDomElement &node) : m_form(0)
	{
		if(node.isNull())
			return;
		m_node = node.attribute(ConstString::node);
		forelements(const QDomElement &elem, node)
		{
			QString name = elem.nodeName();
			if(name == QLatin1String("identity"))
			{
				m_identities.append(Disco::Identity(elem.attribute(QLatin1String("category")),
													elem.attribute(ConstString::type),
													elem.attribute(ConstString::name),
													elem.attribute(ConstString::lang)));
			}
			else if(name == ConstString::feature)
			{
				QString var = elem.attribute(ConstString::var);
				if(!var.isEmpty())
					m_features.insert(var);
			}
			else if(!m_form && name == QLatin1String("x") && elem.namespaceURI() == ConstString::xmlns_data)
			{
				m_form = QSharedPointer<DataForm>(new DataForm(elem));
			}
		}
	}
	
	QDomElement Disco::Info::node(QDomDocument *document) const
	{
		QDomElement node;/* = createElement(document, ConstString::query);
	node.setAttribute(ConstString::xmlns, ConstString::xmlns_disco_info);
	foreach(const Disco::Identity &identity, m_identities)
	{
		QDomElement id = createElement(document, QLatin1String("identity"));
		if(!identity.lang.isEmpty())
			id.setAttribute(ConstString::lang, identity.lang);
		id.setAttribute(QLatin1String("category"), identity.category);
		id.setAttribute(ConstString::type, identity.type);
		if(!identity.name.isEmpty())
			id.setAttribute(ConstString::name, identity.name);
		node.appendChild(id);
	}
	foreach(const QString &feature, m_features)
	{
		QDomElement ftr = createElement(document, ConstString::feature);
		ftr.setAttribute(ConstString::var, feature);
		node.appendChild(ftr);
	}
	if(m_form)
		node.appendChild(m_form->node(document));*/
		return node;
	}
	
	Disco::Items::Items(const QDomElement &node)
	{
		m_node = node.attribute(ConstString::node);
		forelements(const QDomElement &elem, node)
		{
			QString name = elem.nodeName();
			if(name == QLatin1String("item"))
			{
				m_items.append(Disco::Item(elem.attribute(ConstString::jid),
										   elem.attribute(ConstString::node),
										   elem.attribute(ConstString::name)));
			}
		}
	}
	
	QDomElement Disco::Items::node(QDomDocument *document) const
	{
		QDomElement node; /*= createElement(document, ConstString::query);
	node.setAttribute(ConstString::xmlns, ConstString::xmlns_disco_items);
	node.setAttribute(ConstString::node, m_node);
	foreach(const Disco::Item &item, m_items)
	{
		QDomElement elem = createElement(document, QLatin1String("item"));
		elem.setAttribute(ConstString::jid, item.jid);
		elem.setAttribute(ConstString::node, item.node);
		elem.setAttribute(ConstString::name, item.name);
		node.appendChild(elem);
	}*/
		return node;
	}
	
	Disco::Disco(Client *client) : d_ptr(new DiscoPrivate)
	{
		Q_D(Disco);
		d->client = client;
		connect(d->client, SIGNAL(newIQ(IQ)), this, SLOT(handleIQ(IQ)));
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
	
	const QSet<QString> &Disco::features() const
	{
		Q_D(const Disco);
		return d->features;
	}
	
	QSet<QString> &Disco::features()
	{
		Q_D(Disco);
		return d->features;
	}
	
	void Disco::handleIQ(const IQ &iq)
	{
		Q_D(Disco);
		const Info *info = iq.findExtension<Info>().data();
		if(info)
		{
			if(iq.subtype() == IQ::Get)
			{
				IQ receipt(IQ::Result, iq.from(), iq.id());
				receipt.addExtension(new Info(info->node(), d->identities, d->features, d->form));
				d->client->send(receipt);
				iq.accept();
			}
		}
		const Items *items = iq.findExtension<Items>().data();
		if(items)
		{
			if(iq.subtype() == IQ::Get)
			{
				IQ receipt(IQ::Result, iq.from(), iq.id());
				receipt.addExtension(new Items(items->node()));
				d->client->send(receipt);
				iq.accept();
			}
		}
		const SoftwareVersion *version = iq.findExtension<SoftwareVersion>().data();
		if(version)
		{
			if(iq.subtype() == IQ::Get)
			{
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
		QSharedPointer<DataForm> form(new DataForm);
		DataFormFieldList fields;
		fields.append(QSharedPointer<DataFormField>(new DataFormField(QLatin1String("FORM_TYPE"), ConstString::xmlns_softwareinfo, QString(), DataFormField::Hidden)));
		fields.append(QSharedPointer<DataFormField>(new DataFormField(QLatin1String("ip_version"), QStringList()<< QLatin1String("ipv4") << QLatin1String("ipv6"), QString(), DataFormField::None)));
		fields.append(QSharedPointer<DataFormField>(new DataFormField(QLatin1String("os"), os, QString(), DataFormField::None)));
		fields.append(QSharedPointer<DataFormField>(new DataFormField(QLatin1String("software"), name, QString(), DataFormField::None)));
		fields.append(QSharedPointer<DataFormField>(new DataFormField(QLatin1String("software_version"), version, QString(), DataFormField::None)));
		form->setFields(fields);
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
