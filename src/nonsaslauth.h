/****************************************************************************
 *  nonsaslauth.h
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

#ifndef NONSASLAUTH_H
#define NONSASLAUTH_H

#include "streamfeature.h"
#include "stanzaextension.h"

J_BEGIN_NAMESPACE

class IQ;

class JREEN_EXPORT NonSaslAuth : public StreamFeature
{
	J_FEATURE("/stream:features/auth[@xmlns='http://jabber.org/features/iq-auth']")
	Q_OBJECT
public:
	enum Step
	{
		RequestFields,
		ProvideInformation,
		WaitingForResults,
		Completed
	};
	NonSaslAuth();
	int priority() { return 10; }
	void setStreamInfo( StreamInfo *info );
	void reset();
	void processElement( const QDomElement &node );
public slots:
	void handleIq( const IQ &iq, int context );
private:
	class Query : public StanzaExtension
	{
		J_EXTENSION(Query,"/iq/query[@xmlns='jabber:iq:auth']")
	public:
		Query( const QDomElement &node = QDomElement() );
		QDomElement node( QDomDocument *document ) const;
		Query *instance( const JID &jid, const QString &password, const QString &sid ) const;
	private:
		QString m_username;
		QString m_password;
		QString m_resource;
		bool m_is_digest;
	};
	Step m_current_step;
};

J_END_NAMESPACE

#endif // NONSASLAUTH_H
