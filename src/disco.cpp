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

J_BEGIN_NAMESPACE

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

Disco::Info::Info( const QDomElement &node ) : m_form(0)
{
	if( node.isNull() )
		return;
	m_node = node.attribute( ConstString::node );
	forelements( const QDomElement &elem, node )
	{
		QString name = elem.nodeName();
		if( name == identity_str )
		{
			m_identities.append( Disco::Identity( elem.attribute( category_str ),
												  elem.attribute( ConstString::type ),
												  elem.attribute( ConstString::name ),
												  elem.attribute( ConstString::lang ) ) );
		}
		else if( name == ConstString::feature )
		{
			QString var = elem.attribute( ConstString::var );
			if( !var.isEmpty() )
				m_features.insert( var );
		}
		else if( !m_form && name == QLatin1String("x") && elem.namespaceURI() == ConstString::xmlns_data )
		{
			m_form = QSharedPointer<DataForm>( new DataForm( elem ) );
		}
	}
}

QDomElement Disco::Info::node( QDomDocument *document ) const
{
	QDomElement node = createElement( document, ConstString::query );
	node.setAttribute( ConstString::xmlns, ConstString::xmlns_disco_info );
	foreach( const Disco::Identity &identity, m_identities )
	{
		QDomElement id = createElement( document, identity_str );
		if( !identity.lang.isEmpty() )
			id.setAttribute( ConstString::lang, identity.lang );
		id.setAttribute( category_str, identity.category );
		id.setAttribute( ConstString::type, identity.type );
		if( !identity.name.isEmpty() )
			id.setAttribute( ConstString::name, identity.name );
		node.appendChild( id );
	}
	foreach( const QString &feature, m_features )
	{
		QDomElement ftr = createElement( document, ConstString::feature );
		ftr.setAttribute( ConstString::var, feature );
		node.appendChild( ftr );
	}
	if( m_form )
		node.appendChild( m_form->node( document ) );
	return node;
}

Disco::Items::Items( const QDomElement &node )
{
	m_node = node.attribute( ConstString::node );
	forelements( const QDomElement &elem, node )
	{
		QString name = elem.nodeName();
		if( name == item_str )
		{
			m_items.append( Disco::Item( elem.attribute( ConstString::jid ),
										 elem.attribute( ConstString::node ),
										 elem.attribute( ConstString::name ) ) );
		}
	}
}

QDomElement Disco::Items::node( QDomDocument *document ) const
{
	QDomElement node = createElement( document, ConstString::query );
	node.setAttribute( ConstString::xmlns, ConstString::xmlns_disco_items );
	node.setAttribute( ConstString::node, m_node );
	foreach( const Disco::Item &item, m_items )
	{
		QDomElement elem = createElement( document, item_str );
		elem.setAttribute( ConstString::jid, item.jid );
		elem.setAttribute( ConstString::node, item.node );
		elem.setAttribute( ConstString::name, item.name );
		node.appendChild( elem );
	}
	return node;
}

Disco::Disco( Client *client ) : j_ptr(new DiscoPrivate)
{
	J_D(Disco);
	j->client = client;
	j->client->registerStanzaExtension( new Info );
	j->client->registerStanzaExtension( new Items );
	j->features << ConstString::xmlns_disco_info
			<< ConstString::xmlns_disco_items;
	connect( j->client, SIGNAL(newIQ(IQ)), this, SLOT(handleIQ(IQ)) );
}

Disco::~Disco()
{
	delete j_ptr;
}

const Disco::IdentityList &Disco::identities() const
{
	J_D(const Disco);
	return j->identities;
}

Disco::IdentityList &Disco::identities()
{
	J_D(Disco);
	return j->identities;
}

const QSet<QString> &Disco::features() const
{
	J_D(const Disco);
	return j->features;
}

QSet<QString> &Disco::features()
{
	J_D(Disco);
	return j->features;
}

void Disco::handleIQ( const IQ &iq )
{
	J_D(Disco);
	const Info *info = iq.findExtension<Info>().data();
	if( info )
	{
		if( iq.subtype() == IQ::Get )
		{
			IQ receipt( IQ::Result, iq.from(), iq.id() );
			receipt.addExtension( new Info( info->node(), j->identities, j->features, j->form ) );
			j->client->send( receipt );
			iq.accept();
		}
	}
	const Items *items = iq.findExtension<Items>().data();
	if( items )
	{
		if( iq.subtype() == IQ::Get )
		{
			IQ receipt( IQ::Result, iq.from(), iq.id() );
			receipt.addExtension( new Items( items->node() ) );
			j->client->send( receipt );
			iq.accept();
		}
	}
	const SoftwareVersion *version = iq.findExtension<SoftwareVersion>().data();
	if( version )
	{
		if( iq.subtype() == IQ::Get )
		{
			IQ receipt( IQ::Result, iq.from(), iq.id() );
			receipt.addExtension( new SoftwareVersion( j->software_name, j->software_version, j->os ) );
			j->client->send( receipt );
			iq.accept();
		}
	}
}

void Disco::setSoftwareVersion( const QString &name, const QString &version, const QString &os )
{
	J_D(Disco);
	j->software_name = name;
	j->software_version = version;
	j->os = os;
	QSharedPointer<DataForm> form( new DataForm );
	DataFormFieldList fields;
	fields.append( QSharedPointer<DataFormField>( new DataFormField(FORM_TYPE_str, ConstString::xmlns_softwareinfo, QString(), DataFormField::Hidden ) ) );
	fields.append( QSharedPointer<DataFormField>( new DataFormField(ip_version_str, QStringList()<< ipv4_str << ipv6_str, QString(), DataFormField::None ) ) );
	fields.append( QSharedPointer<DataFormField>( new DataFormField(os_str, os, QString(), DataFormField::None ) ) );
	fields.append( QSharedPointer<DataFormField>( new DataFormField(software_str, name, QString(), DataFormField::None ) ) );
	fields.append( QSharedPointer<DataFormField>( new DataFormField(software_version_str, version, QString(), DataFormField::None ) ) );
	form->setFields( fields );
	j->form = form;
}

const DataForm *Disco::form() const
{
	J_D(const Disco);
	return j->form.data();
}

void Disco::setForm( DataForm *form )
{
	J_D(Disco);
	j->form = QSharedPointer<DataForm>( form );
}

J_END_NAMESPACE
