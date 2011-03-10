/*****************************************************************************
 *  nicknamefactory.cpp
 *
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/
#include "nicknamefactory.h"
#include <QStringList>

#define NS_NICKNAME QLatin1String("http://jabber.org/protocol/nick")

namespace Jreen {

class NicknameFactoryPrivate
{
public:
	QString nickname;
};

NicknameFactory::NicknameFactory() :
	d_ptr(new NicknameFactoryPrivate)
{
}

QStringList NicknameFactory::features() const
{
	return QStringList(NS_NICKNAME);
}

bool NicknameFactory::canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(uri);
	Q_UNUSED(attributes);
	return name == QLatin1String("nick") && uri == NS_NICKNAME;
}

void NicknameFactory::handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(name);
	Q_UNUSED(uri);
	Q_UNUSED(attributes);
}

void NicknameFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	Q_UNUSED(name);
	Q_UNUSED(uri);
}

void NicknameFactory::handleCharacterData(const QStringRef &text)
{
	d_func()->nickname = text.toString();
}

void NicknameFactory::serialize(StanzaExtension *extension, QXmlStreamWriter *writer)
{
	Nickname *nick = se_cast<Nickname*>(extension);
	writer->writeStartElement(QLatin1String("nick"));
	writer->writeDefaultNamespace(NS_NICKNAME);
	writer->writeCharacters(nick->nick());
	writer->writeEndElement();
}

StanzaExtension::Ptr NicknameFactory::createExtension()
{
	return StanzaExtension::Ptr(new Nickname(d_func()->nickname));
}

NicknameFactory::~NicknameFactory()
{

}

} // namespace Jreen
