/*****************************************************************************
 *  nicknamefactory.h
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
#ifndef NICKNAMEFACTORY_H
#define NICKNAMEFACTORY_H
#include "stanzafactory.h"
#include "nickname.h"

namespace Jreen {

class NicknameFactoryPrivate;
class NicknameFactory : public StanzaExtensionFactory<Nickname>
{
	Q_DECLARE_PRIVATE(NicknameFactory)
public:
    NicknameFactory();
	virtual ~NicknameFactory();
	QStringList features() const;
	bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleEndElement(const QStringRef &name, const QStringRef &uri);
	void handleCharacterData(const QStringRef &text);
	void serialize(StanzaExtension *extension, QXmlStreamWriter *writer);
	StanzaExtension::Ptr createExtension();
private:
	QScopedPointer<NicknameFactoryPrivate> d_ptr;
};

} // namespace Jreen

#endif // NICKNAMEFACTORY_H
