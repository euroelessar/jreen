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

namespace Jreen
{

class IQ;

class JREEN_EXPORT NonSaslAuth : public QObject, public StreamFeature
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
	void setStreamInfo(StreamInfo *info);
	void reset();
	bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleEndElement(const QStringRef &name, const QStringRef &uri);
	void handleCharacterData(const QStringRef &text);
	bool isActivatable();
	bool activate();
public slots:
	void handleIq(const IQ &iq, int context);
private:
	class Query : public Payload
	{
		J_PAYLOAD(Jreen::NonSaslAuth::Query)
	public:
		Query();
		Query *instance(const JID &jid, const QString &password, const QString &sid) const;
	private:
		QString m_username;
		QString m_password;
		QString m_resource;
		bool m_is_digest;
	};
	Step m_current_step;
};

}

#endif // NONSASLAUTH_H
