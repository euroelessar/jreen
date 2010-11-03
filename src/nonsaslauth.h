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

namespace jreen
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
	class Query : public StanzaExtension
	{
		J_EXTENSION(jreen::NonSaslAuth::Query,"/iq/query[@xmlns='jabber:iq:auth']")
	public:
		Query(const QDomElement &node = QDomElement());
		QDomElement node(QDomDocument *document) const;
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
//template <>                                                         
//struct QMetaTypeId< jreen::NonSaslAuth::Query* >                                          
//{                                                                   
//	enum { Defined = 1 };                                           
//	static int qt_metatype_id()                                     
//		{                                                           
//			static QBasicAtomicInt metatype_id = Q_BASIC_ATOMIC_INITIALIZER(0); 
//			if (!metatype_id)                                       
//				metatype_id = qRegisterMetaType< jreen::NonSaslAuth::Query* >("jreen::NonSaslAuth::Query*",      
//						   reinterpret_cast< jreen::NonSaslAuth::Query* *>(quintptr(-1))); 
//			return metatype_id;                                     
//		}                                                           
//};                                                                  
//Q_DECLARE_METATYPE(jreen::NonSaslAuth::Query*)
//J_DECLARE_EXTENSION(jreen::NonSaslAuth::Query)

#endif // NONSASLAUTH_H
