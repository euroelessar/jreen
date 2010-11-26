/****************************************************************************
 *  jstrings.h
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

#ifndef JSTRINGS_H
#define JSTRINGS_H

#include "jreen.h"

namespace jreen {
namespace ConstString
{
//static const QString from                   (QLatin1String("from"));
//static const QString to                     (QLatin1String("to"));
//static const QString id                     (QLatin1String("id"));
//static const QString jid                    (QLatin1String("jid"));
//static const QString type                   (QLatin1String("type"));
//static const QString xmlns                  (QLatin1String("xmlns"));
//static const QString value                  (QLatin1String("value"));
//static const QString query                  (QLatin1String("query"));
//static const QString node                   (QLatin1String("node"));
//static const QString item                   (QLatin1String("item"));
//static const QString name                   (QLatin1String("name"));
//static const QString lang                   (QLatin1String("xml:lang"));
//static const QString feature                (QLatin1String("feature"));
//static const QString var                    (QLatin1String("var"));
//static const QString ns_etherx              (QLatin1String("http://etherx.jabber.org/streams"));
//static const QString ns_client              (QLatin1String("jabber:client"));
static const QString ns_server              (QLatin1String("jabber:server"));
static const QString ns_dialback            (QLatin1String("jabber:server:dialback"));
static const QString ns_streams             (QLatin1String("urn:ietf:params:xml:ns:xmpp-streams"));
static const QString ns_tls                 (QLatin1String("urn:ietf:params:xml:ns:xmpp-tls"));
static const QString ns_sasl                (QLatin1String("urn:ietf:params:xml:ns:xmpp-sasl"));
static const QString ns_session             (QLatin1String("urn:ietf:params:xml:ns:xmpp-session"));
static const QString ns_stanzas             (QLatin1String("urn:ietf:params:xml:ns:xmpp-stanzas"));
static const QString ns_bind                (QLatin1String("urn:ietf:params:xml:ns:xmpp-bind"));
static const QString ns_compress_features   (QLatin1String("http://jabber.org/features/compress"));
static const QString ns_compress_protocol   (QLatin1String("http://jabber.org/protocol/compress"));
static const QString ns_hosts               (QLatin1String("http://barracuda.com/xmppextensions/hosts"));
static const QString xmlns_delay            (QLatin1String("jabber:x:delay"));
static const QString xmlns_version          (QLatin1String("jabber:iq:version"));
static const QString xmlns_caps             (QLatin1String("http://jabber.org/protocol/caps"));
static const QString xmlns_disco_info       (QLatin1String("http://jabber.org/protocol/disco#info"));
static const QString xmlns_disco_items      (QLatin1String("http://jabber.org/protocol/disco#items"));
static const QString xmlns_disco_publish    (QLatin1String("http://jabber.org/protocol/disco#publish"));
static const QString xmlns_data             (QLatin1String("jabber:x:data"));
static const QString xmlns_roster           (QLatin1String("jabber:iq:roster"));
static const QString xmlns_private_xml      (QLatin1String("jabber:iq:private"));
static const QString xmlns_qutim_img        (QLatin1String("qutim:img"));
static const QString xmlns_nickname         (QLatin1String("http://jabber.org/protocol/nick"));
static const QString xmlns_receipts         (QLatin1String("urn:xmpp:receipts"));
static const QString xmlns_softwareinfo     (QLatin1String("urn:xmpp:dataforms:softwareinfo"));
}

//enums
template<typename T, int N>
Q_INLINE_TEMPLATE int strToEnum(const T &str, const char *(&strings)[N])
{
	for(int i=0;i<=N;i++) {
		if(QLatin1String(strings[i]) == str)
			return i;
	}
	return -1;
}

template<typename X,typename T, int N>
Q_INLINE_TEMPLATE X strToEnum(const T &str, const char *(&strings)[N])
{
	return static_cast<X>(strToEnum(str,strings));
}

template<int N>
Q_INLINE_TEMPLATE QLatin1String enumToStr(int i, const char *(&strings)[N])
{
	if(i<0 || i>=N)
		return QLatin1String(0);
	return QLatin1String(strings[i]);
}

//flags
template<typename T, int N>
Q_INLINE_TEMPLATE int strToFlag(const T &str, const char *(&strings)[N])
{
	int flag = 0x1;
	for(int i=0;i<=N;i++) {
		if(QLatin1String(strings[i]) == str)
			return i;
		flag <<= 1;
	}
	return -1;
}

template<typename T, int N, typename X>
Q_INLINE_TEMPLATE int strToFlag(const T &str, const char *(&strings)[N])
{
	return static_cast<X>(strToFlag(str,strings));
}

template<int N>
Q_INLINE_TEMPLATE QString flagToStr(int i, const char *(&strings)[N])
{
	int n=1;
	while(n < i)
		n <<= 1;
	if(n<0 || n>=N)
		return QString();
	return QLatin1String(strings[n]);
}


}

#endif // JSTRINGS_H
