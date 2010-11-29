/****************************************************************************
 *  capabilities.cpp
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

#include <QStringList>
#include <QMap>
#include <QStringBuilder>
#include <QXmlStreamWriter>
#include "capabilities.h"
#include "disco.h"
#include "jstrings.h"

namespace jreen
{

J_STRING(ver)
J_STRING(hash)

Capabilities::Capabilities(const QString &ver) :
	m_ver(ver),
	m_node(QLatin1String("http://jreen.org/"))
{
}

//Capabilities::Capabilities(const QDomElement &node) : m_disco(0)
//{
////	if(node.isNull())
////		return;
////	m_node = node.attribute(ConstString::node);
////	m_ver = node.attribute(ver_str);
//}

//void Capabilities::writeXml(QXmlStreamWriter *writer) const
//{
////	writer->writeStartElement(QLatin1String("c"));
////	writer->writeAttribute(ConstString::xmlns, ConstString::xmlns_caps);
////	writer->writeAttribute(hash_str, QLatin1String("sha-1"));
////	writer->writeAttribute(ConstString::node, m_node);
////	writer->writeAttribute(ver_str, ver());
////	writer->writeEndElement();
//}

//QString Capabilities::ver() const
//{
//	//	if(!m_disco)
//	//		return m_ver;
//	//	QString s;
//	//	QStringList sl;
//	//	const Disco::IdentityList &identity_list = m_disco->identities();
//	//	foreach(const Disco::Identity &i, identity_list)
//	//		sl << (i.category % QLatin1Char('/') % i.type
//	//			   % QLatin1Char('/') % i.lang % QLatin1Char('/') % i.name);
//	//	sl.sort();
//	//	foreach(const QString &str, sl)
//	//		s.append(str);
//	//	sl = m_disco->features().values();
//	//	sl.sort();
//	//	foreach(const QString &str, sl)
//	//		s.append(str).append(QLatin1Char('<'));
//	//	const DataForm *data = m_disco->form();
//	//	QString form_type;
//	//	QMap<QString,QStringList> fields;
//	//	foreach(const QSharedPointer<DataFormField> &field, data->fields())
//	//	{
//	//		if(field->var() == QLatin1String("FORM_TYPE"))
//	//			form_type = field->values().first();
//	//		else
//	//			fields.insert(field->var(), field->values());
//	//	}
//	//	s.append(form_type).append(QLatin1Char('<'));
//	//	QMap<QString,QStringList>::iterator it = fields.begin();
//	//	for(; it != fields.end(); it++)
//	//	{
//	//		s.append(it.key()).append(QLatin1Char('<'));
//	//		foreach(const QString &value, it.value())
//	//			s.append(value).append(QLatin1Char('<'));
//	//	}
//	//	return QString::fromLatin1(QCryptographicHash::hash(s.toUtf8(), QCryptographicHash::Sha1).toBase64());
//}

}
