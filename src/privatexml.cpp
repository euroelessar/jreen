/****************************************************************************
 *  privatexml.cpp
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

#include "privatexml_p.h"

J_BEGIN_NAMESPACE

PrivateXml::PrivateXml( Client *client ) : QObject(client), j_ptr(new PrivateXmlPrivate)
{
	J_D(PrivateXml);
	j->client = client;
	j->client->registerStanzaExtension( new Query );
}

void PrivateXml::request( const QString &name, const QString &xmlns, QObject *handler, const char *member )
{
	J_D(PrivateXml);
	QString id = j->client->getID();
	IQ iq( IQ::Get, JID(), id );
	iq.addExtension( new Query( name, xmlns ) );
	j->tracks.insert( id, new PrivateXmlTrack( handler, member ) );
	j->client->send( iq, this, SLOT(handleIQ(IQ,int)), Request );
}

void PrivateXml::store( const QDomElement &node, QObject *handler, const char *member )
{
	J_D(PrivateXml);
	QString id = j->client->getID();
	IQ iq( IQ::Get, JID(), id );
	iq.addExtension( new Query( node ) );
	j->tracks.insert( id, new PrivateXmlTrack( handler, member ) );
	j->client->send( iq, this, SLOT(handleIQ(IQ,int)), Store );
}

void PrivateXml::handleIQ( const IQ &iq, int context )
{
	J_D(PrivateXml);
	PrivateXmlTrack *track = j->tracks.take( iq.id() );
	if( !track )
		return;
	const QSharedPointer<Error> error = iq.findExtension<Error>();
	const QSharedPointer<Query> query = iq.findExtension<Query>();
	bool is_error = !query;
	if( query )
	{
		if( iq.subtype() == IQ::Result )
			track->newResult( query->xml(), context == Store ? StoreOk : RequestOk, error );
		else if( iq.subtype() == IQ::Error )
			is_error = true;
	}
	if( is_error )
		track->newResult( QDomElement(), context == Store ? StoreError : RequestError, error );
	delete track;
}

J_END_NAMESPACE
