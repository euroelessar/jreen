/****************************************************************************
**
** Jreen
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
**
*****************************************************************************
**
** $JREEN_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $JREEN_END_LICENSE$
**
****************************************************************************/
#include "nicknamefactory_p.h"
#include <QStringList>

#define NS_NICKNAME QLatin1String("http://jabber.org/protocol/nick")

namespace Jreen {

class JREEN_AUTOTEST_EXPORT NicknameFactoryPrivate
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

void NicknameFactory::serialize(Payload *extension, QXmlStreamWriter *writer)
{
	Nickname *nick = se_cast<Nickname*>(extension);
	writer->writeStartElement(QLatin1String("nick"));
	writer->writeDefaultNamespace(NS_NICKNAME);
	writer->writeCharacters(nick->nick());
	writer->writeEndElement();
}

Payload::Ptr NicknameFactory::createPayload()
{
	return Payload::Ptr(new Nickname(d_func()->nickname));
}

NicknameFactory::~NicknameFactory()
{

}

} // namespace Jreen
