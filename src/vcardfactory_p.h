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

#ifndef VCARDFACTORY_P_H
#define VCARDFACTORY_P_H
#include "vcard_p.h"
#include "stanzaextension.h"
#include <QPair>

namespace Jreen {
class JREEN_AUTOTEST_EXPORT VCardFactoryPrivate;

class AbstractStructureParser : public XmlStreamParser
{
public:
	AbstractStructureParser(const QLatin1String &name);
	bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleEndElement(const QStringRef &name, const QStringRef &uri);
	void handleCharacterData(const QStringRef &text);
protected:
	void serialize(void *zero, void *data, QXmlStreamWriter *writer);
	void addByteArray(const QLatin1String &name, QByteArray *str);
	void addString(const QLatin1String &name, QString *str);
	void addFlag(const char **table, int size, int *value);
	template <int N>
	void addFlag(const char *(&table)[N], int *value)
	{ addFlag(table, N, value); }

	int m_depth;
	
private:
	struct FlagInfo
	{
		const char **table;
		int tableSize;
		int *value;
	};

	QList<QPair<QLatin1String, QString*> > m_strings;
	QList<QPair<QLatin1String, QByteArray*> > m_byteArrays;
	QList<FlagInfo> m_flags;
	QLatin1String m_name;
	QString *m_currentString;
	QByteArray *m_currentArray;
};

template <typename T>
class StructureParser : public AbstractStructureParser
{
public:
	StructureParser(const QLatin1String &name) : AbstractStructureParser(name) {}
	T create() { return m_data; }
	void serialize(const T &data, QXmlStreamWriter *writer) 
	{ AbstractStructureParser::serialize(&m_data, const_cast<T*>(&data), writer); }

protected:
	T m_data;
};

template <typename TPrivate, typename T>
class StructurePrivateParser : public AbstractStructureParser
{
public:
	StructurePrivateParser(const QLatin1String &name) : AbstractStructureParser(name) {}
	T create() { return T(*new TPrivate(m_data)); }
	void serialize(const T &data, QXmlStreamWriter *writer) 
	{
		TPrivate *t1 = &m_data;
		TPrivate *t2 = const_cast<TPrivate*>(TPrivate::get(&data));
		AbstractStructureParser::serialize(t1, t2, writer);
	}

protected:
	TPrivate m_data;
};

class JREEN_AUTOTEST_EXPORT VCardFactory : public PayloadFactory<VCard>
{
	Q_DECLARE_PRIVATE(VCardFactory)
public:
    VCardFactory();
    virtual ~VCardFactory();
	QStringList features() const;
	bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleEndElement(const QStringRef &name, const QStringRef &uri);
	void handleCharacterData(const QStringRef &text);
	void serialize(Payload *extension, QXmlStreamWriter *writer);
	Payload::Ptr createPayload();	
private:
	QScopedPointer<VCardFactoryPrivate> d_ptr;
	enum State {
		AtRoot,
		AtName,
		AtPhoto,
		AtTelephone,
		AtEMail,
		AtAddress,
		AtOrg,
		AtNowhere,
		LastState
	};
};

} // namespace Jreen

#endif // VCARDFACTORY_P_H
