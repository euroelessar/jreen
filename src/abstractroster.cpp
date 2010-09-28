/****************************************************************************
 *  abstractroster.cpp
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

#include "abstractroster.h"
#include "client.h"
#include "jstrings.h"
#include "iq.h"
#include "jid.h"

J_BEGIN_NAMESPACE

J_STRING(group)
J_STRING(subscription)
J_STRING(ask)

static const QStringList roster_subscriptions = QStringList()
												<< ConstString::from << ConstString::to
												<< QLatin1String("both") << QLatin1String("remove");

AbstractRoster::Query::Query( AbstractRoster *roster, const QDomElement &node ) : m_roster(roster)
{
	if( node.isNull() || !m_roster )
		return;
	forelements( const QDomElement &item_node, node )
	{
		if( item_node.nodeName() == ConstString::item )
		{
			m_items << QSharedPointer<AbstractRosterItem>( m_roster->createItem( item_node ) );
		}
	}
}

QDomElement AbstractRoster::Query::node( QDomDocument *document ) const
{
	QDomElement node = createElement( document, ConstString::query );
	node.setAttribute( ConstString::xmlns, ConstString::xmlns_roster );
	foreach( const QSharedPointer<AbstractRosterItem> &item, m_items )
	{
		node.appendChild( item->node( document ) );
	}
	return node;
}

AbstractRosterItem::AbstractRosterItem( const QDomElement &node, AbstractRoster *roster, AbstractRosterItemPrivate *data )
{
	m_roster = roster;
	j_ptr = data ? data : new AbstractRosterItemPrivate();
	if( node.isNull() )
		return;
	J_D(AbstractRosterItem);
	j->groups.clear();
	forelements( const QDomElement &elem, node )
	{
		if( elem.nodeName() == group_str )
		{
			j->groups << elem.text();
		}
	}
	j->ask = node.attribute( ask_str );
	int sub = roster_subscriptions.indexOf( node.attribute( subscription_str ) );
	j->subscription = sub == -1 ? None : static_cast<SubscriptionType>( sub );
	j->jid = node.attribute( ConstString::jid );
	j->name = node.attribute( ConstString::name );
}

AbstractRosterItem::~AbstractRosterItem()
{
	if( j_ptr )
		delete j_ptr;
}

QDomElement AbstractRosterItem::node( QDomDocument *doc ) const
{
	J_D(const AbstractRosterItem);
	QDomElement node = createElement( doc, ConstString::item );
	node.setAttribute( ConstString::jid, j->jid );
	node.setAttribute( subscription_str, j->subscription == None ? QString() : roster_subscriptions[j->subscription] );
	if( j->subscription != Remove )
	{
		if( !j->ask.isEmpty() )
			node.setAttribute( ask_str, j->ask );
		if( !j->name.isEmpty() )
			node.setAttribute( ConstString::name, j->name );
		foreach( const QString &group, j->groups )
		{
			node.appendChild( createElement( doc, group_str, group ) );
		}
	}
	return node;
}

void AbstractRosterItem::setData( const QSharedPointer<AbstractRosterItem> &item )
{
	J_D(AbstractRosterItem);
	const AbstractRosterItemPrivate *d = item->j_ptr;
	j->ask = d->ask;
	j->groups = d->groups;
	j->subscription = d->subscription;
	j->name = d->name;
}

 AbstractRoster::AbstractRoster( Client *client, AbstractRosterPrivate *data ) : QObject(client), j_ptr(data?data:new AbstractRosterPrivate)
 {
	 j_ptr->client = client;
	 m_self = createItem();
	 AbstractRosterItemPrivate *j = m_self->j_ptr;
	 j->jid = client->jid().bare();
	 j->subscription = AbstractRosterItem::Both;
	 connect( client, SIGNAL(newPresence(Presence)), this, SLOT(handlePresence(Presence)) );
	 init();
 }

void AbstractRoster::load()
{
	J_D(AbstractRoster);
	IQ iq( IQ::Get, JID(), j->client->getID() );
	iq.addExtension( new Query( this ) );
	j->client->send( iq, this, SLOT(handleIQ(IQ,int)), LoadRoster );
}

void AbstractRoster::synchronize()
{
	J_D(AbstractRoster);
	foreach( const QSharedPointer<AbstractRosterItem> &item, m_changed_items )
	{
		IQ iq( IQ::Set, JID() );
		iq.addExtension( new Query( item ) );
		j->client->send( iq, this, SLOT(handleIQ(IQ,int)), SyncContext );
	}
	m_changed_items.clear();
}

void AbstractRoster::init()
{
	J_D(AbstractRoster);
	j->client->registerStanzaExtension( new Query( this ) );
}

void AbstractRoster::add( const JID &jid, const QString &name, const QStringList &groups )
{
	if( !jid.isValid() )
		return;
	J_D(AbstractRoster);
	QSharedPointer<AbstractRosterItem> item( createItem() );
	AbstractRosterItemPrivate *d = item->j_ptr;
	d->jid = jid;
	d->subscription = AbstractRosterItem::None;
	d->name = name;
	d->groups = groups;
	IQ iq( IQ::Set, JID() );
	iq.addExtension( new Query( item ) );
	j->client->send( iq, this, SLOT(handleIQ(IQ,int)), AddRosterItem );
}

void AbstractRoster::remove( const JID &jid )
{
	if( !jid.isValid() )
		return;
	J_D(AbstractRoster);
	QSharedPointer<AbstractRosterItem> item( createItem() );
	item->j_ptr->jid = jid;
	IQ iq( IQ::Set, JID() );
	iq.addExtension( new Query( item ) );
	j->client->send( iq, this, SLOT(handleIQ(IQ,int)), AddRosterItem );
}

QSharedPointer<AbstractRosterItem> AbstractRoster::createItem( const QDomElement &node )
{
	return QSharedPointer<AbstractRosterItem>( new AbstractRosterItem( node, this ) );
}

QSharedPointer<AbstractResource> AbstractRoster::createResource()
{
	return QSharedPointer<AbstractResource>( new AbstractResource() );
}

void AbstractRoster::handleIQ( const IQ &iq )
{
	const Query *roster = iq.findExtension<Query>().data();
	if( !roster )
		return;
	foreach( const QSharedPointer<AbstractRosterItem> &item, roster->items() )
	{
		if( item->subscriptionType() == AbstractRosterItem::Remove )
		{
			m_items.remove( item->jid() );
			onItemRemoved( item->jid() );
		}
		else
		{
			QHash<QString, QSharedPointer<AbstractRosterItem> >::iterator item_iter = m_items.find( iq.from().bare() );
			if( item_iter == m_items.end() )
			{
				m_items.insert( item_iter.value()->jid(), item_iter.value() );
				onItemAdded( item_iter.value() );
			}
			else
			{
				item_iter.value()->setData( item );
				onItemUpdated( item_iter.value() );
			}
		}
	}
}

void AbstractRoster::handleIQ( const IQ &iq, int context )
{
	switch( context )
	{
	case LoadRoster:{
		m_items.clear();
		const Query *roster = iq.findExtension<Query>().data();
		onLoaded( roster->items() );
		break;}
	default:
		break;
	}
}

void AbstractRoster::onLoaded( const QList<QSharedPointer<AbstractRosterItem> > &items )
{
	foreach( const QSharedPointer<AbstractRosterItem> &item, items )
	{
		m_items.insert( item->jid(), item );
	}
	emit loaded();
}

void AbstractRoster::handlePresence( const Presence &presence )
{
	QSharedPointer<AbstractRosterItem> item;
	if( presence.from().bare() == m_self->jid() )
	{
		item = m_self;
	}
	else
	{
		QHash<QString, QSharedPointer<AbstractRosterItem> >::iterator item_iter = m_items.find( presence.from().bare() );
		if( item_iter == m_items.end() )
		{
			emit nonRosterPresence( presence );
			return;
		}
		item = item_iter.value();
	}
	if( presence.subtype() == Presence::Unavailable )
	{
		item->m_resources.remove( presence.from().resource() );
	}
	else
	{
		QSharedPointer<AbstractResource> resource = item->resource( presence.from().resource() );
		if( !resource )
		{
			resource = QSharedPointer<AbstractResource>( createResource() );
			resource->j_ptr->resource = presence.from().resource();
			item->m_resources.insert( resource->j_ptr->resource, resource );
		}
		resource->j_ptr->presence = presence;
	}
	if( item == m_self )
		emit selfPresence( item, presence );
	else
		emit rosterPresence( item, presence );
}

J_END_NAMESPACE
