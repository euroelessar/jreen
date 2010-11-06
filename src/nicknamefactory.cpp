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

namespace jreen {

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
	return name == QLatin1String("nick") && uri == NS_NICKNAME;
}

void NicknameFactory::handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{

}

void NicknameFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{

}

void NicknameFactory::handleCharacterData(const QStringRef &text)
{

}

void NicknameFactory::serialize(StanzaExtension *extension, QXmlStreamWriter *writer)
{
	Nickname *nick = se_cast<Nickname*>(extension);
	writer->writeDefaultNamespace(NS_NICKNAME);
	writer->writeTextElement(QLatin1String("nickname"),nick->nick());
}

StanzaExtension::Ptr NicknameFactory::createExtension()
{
	return StanzaExtension::Ptr(new Nickname(d_func()->nickname));
}

NicknameFactory::~NicknameFactory()
{

}

} // namespace jreen
