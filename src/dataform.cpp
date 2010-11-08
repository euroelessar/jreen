/****************************************************************************
 *  dataform.cpp
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

#include "dataform.h"
#include "jstrings.h"
#include <QStringList>

namespace jreen
{

J_STRING(field)
J_STRING(option)
J_STRING(label)
J_STRING(desc)
J_STRING(required)

static const QStringList dataformfield_types = QStringList()
											   << QLatin1String("boolean") << QLatin1String("fixed")
											   << QLatin1String("hidden") << QLatin1String("jid-multi")
											   << QLatin1String("jid-single") << QLatin1String("list-multi")
											   << QLatin1String("list-single") << QLatin1String("text-multi")
											   << QLatin1String("text-private") << QLatin1String("text-single")
											   << QString();

static const QStringList dataform_types = QStringList()
										  << QLatin1String("form") << QLatin1String("submit")
										  << QLatin1String("cancel") << QLatin1String("result");

static inline DataFormField::Type getDataFormFieldType(const QString &type)
{
	int t = dataformfield_types.indexOf(type);
	return t < 0 ? DataFormField::Invalid : static_cast<DataFormField::Type>(t);
}

DataFormField::DataFormField(const QDomElement &node)
{
}

QDomElement DataFormField::node(QDomDocument *doc) const
{
	if(m_type == Invalid)
		return QDomElement();
	QDomElement node;/* = createElement(doc, field_str);
	if(m_type != None)
		node.setAttribute(ConstString::type, dataformfield_types.at(m_type));
	node.setAttribute(ConstString::var, m_var);
	if(!m_label.isEmpty())
		node.setAttribute(label_str, m_label);
	if(m_required)
		createElement(node, required_str);
	if(!m_desc.isEmpty())
		createElement(node, desc_str, m_desc);
	foreach(const QString &value, m_values)
		createElement(node, ConstString::value, value);
	foreach(const QSharedPointer<DataFormOption> &option, m_options)
	{
		QDomElement opt = createElement(node, option_str);
		opt.setAttribute(label_str, option->label);
		createElement(opt, ConstString::value, option->value);
	}*/
	return node;
}

DataForm::DataForm(const QDomElement &node)
{
	m_form_type = Result;
	Q_UNUSED(node);
}

QDomElement DataForm::node(QDomDocument *document) const
{
	if(m_form_type == Invalid)
		return QDomElement();
	QDomElement node;/* = createElement(document, QLatin1String("x"));
	node.setAttribute(ConstString::type, dataform_types.at(m_form_type));
	node.setAttribute(ConstString::xmlns, ConstString::xmlns_data);
	foreach(const QSharedPointer<DataFormField> &field, m_fields_list)
		node.appendChild(field->node(document));*/
	return node;
}

enum DataFormState { AtStart, AtResource, AtJid };

class DataFormFactoryPrivate
{
public:
	int depth;
	DataFormState state;
	QString title;
	DataFormReportedList reported;
	DataFormItemList items;
	DataForm::Type formType;
};

DataFormFactory::DataFormFactory() : d_ptr(new DataFormFactoryPrivate)
{
	Q_D(DataFormFactory);
	d->state = AtStart;
	d->depth = 0;
	d->formType = DataForm::Invalid;
}

DataFormFactory::~DataFormFactory()
{
}

QStringList DataFormFactory::features() const
{
	return QStringList(QLatin1String("jabber:x:data"));
}

bool DataFormFactory::canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(attributes);
	return name == QLatin1String("x") && uri == QLatin1String("jabber:x:data");
}

void DataFormFactory::handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
}

void DataFormFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
}

void DataFormFactory::handleCharacterData(const QStringRef &text)
{
}

void DataFormFactory::serialize(StanzaExtension *extension, QXmlStreamWriter *writer)
{
}

StanzaExtension::Ptr DataFormFactory::createExtension()
{
	DataForm *form = new DataForm;
//	form->setFields();
	return StanzaExtension::Ptr(form);
}

}
