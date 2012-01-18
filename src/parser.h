/****************************************************************************
**
** Jreen
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef PARSER_H
#define PARSER_H

#include "stanza.h"
#include <QXmlStreamReader>
#include <QEvent>

namespace Jreen
{
class Client;
class ParserPrivate;

class Parser : public QObject, public XmlStreamParser
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(Parser)
public:
	enum State
	{
		WaitingForStanza,
		ReadFeatures,
		ReadStanza,
		ReadCustom
	};

	Parser(Client *client);
	~Parser();
	void reset();
	void activateFeature();
	QByteArray nextPart(QByteArray &data, bool first, bool *needMoreData);
	
	bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleEndElement(const QStringRef &name, const QStringRef &uri);
	void handleCharacterData(const QStringRef &text);
	
	bool event(QEvent *ev);
	void appendData(const QByteArray &a);
	void parseData();
private:
	QScopedPointer<ParserPrivate> d_ptr;
};
}

#endif
