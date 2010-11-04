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

#include "disco.h"
#include "iq.h"
#include "jstrings.h"
#include "dataform.h"
#include "softwareversion.h"

namespace jreen
{

J_STRING(category)
J_STRING(item)
J_STRING(identity)
J_STRING(FORM_TYPE)
J_STRING(ip_version)
J_STRING(os)
J_STRING(software)
J_STRING(software_version)
J_STRING(ipv4)
J_STRING(ipv6)

struct DiscoPrivate
{
	Disco::IdentityList identities;
	QSet<QString> features;
	Client *client;
	QSharedPointer<DataForm> form;
	QString os;
	QString software_name;
	QString software_version;
};

Disco::Info::Info(const QDomElement &node) : m_form(0)
{
	if(node.isNull())
		return;
	m_node = node.attribute(ConstString::node);
	forelements(const QDomElement &elem, node)
	{
		QString name = elem.nodeName();
		if(name == identity_str)
		{
			m_identities.append(Disco::Identity(elem.attribute(category_str),
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
		QDomElement id = createElement(document, identity_str);
		if(!identity.lang.isEmpty())
			id.setAttribute(ConstString::lang, identity.lang);
		id.setAttribute(category_str, identity.category);
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
		if(name == item_str)
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
		QDomElement elem = createElement(document, item_str);
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
	d->client->registerStanzaExtension(new Info);
	d->client->registerStanzaExtension(new Items);
	d->features << ConstString::xmlns_disco_info
			<< ConstString::xmlns_disco_items;
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
	fields.append(QSharedPointer<DataFormField>(new DataFormField(FORM_TYPE_str, ConstString::xmlns_softwareinfo, QString(), DataFormField::Hidden)));
	fields.append(QSharedPointer<DataFormField>(new DataFormField(ip_version_str, QStringList()<< ipv4_str << ipv6_str, QString(), DataFormField::None)));
	fields.append(QSharedPointer<DataFormField>(new DataFormField(os_str, os, QString(), DataFormField::None)));
	fields.append(QSharedPointer<DataFormField>(new DataFormField(software_str, name, QString(), DataFormField::None)));
	fields.append(QSharedPointer<DataFormField>(new DataFormField(software_version_str, version, QString(), DataFormField::None)));
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
