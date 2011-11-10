/****************************************************************************
**
** Jreen
**
** Copyright (C) 2011 Sidorov Aleksey <sauron@citadelspb.com>
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

#include "dataformfactory_p.h"
#include "jstrings.h"
#include <QXmlStreamWriter>
#include "util.h"
#include "multimediadatafactory_p.h"
#include <QDebug>
#define NS_DATAFORM QLatin1String("jabber:x:data")

namespace Jreen {

using namespace Util;

class DataFormOptionParser : XmlStreamFactory<const QPair<QString, QString> >
{
public:
	DataFormOptionParser()
	{
		m_depth = 0;
		m_atValue = 0;
		clear();
	}
	virtual ~DataFormOptionParser()
	{

	}
	virtual bool canParse(const QStringRef &name, const QStringRef &uri,
						  const QXmlStreamAttributes &attributes)
	{
		Q_UNUSED(uri);
		Q_UNUSED(attributes);
		return name == QLatin1String("option");
	}
	virtual void handleStartElement(const QStringRef &name, const QStringRef &uri,
									const QXmlStreamAttributes &attributes)
	{
		Q_UNUSED(name);
		Q_UNUSED(uri);
		m_depth++;
		if(m_depth == 1) {
			m_value.clear();
			m_label = attributes.value(QLatin1String("label")).toString();
		} else if (m_depth == 2 && name == QLatin1String("value")) {
			m_atValue = 1;
		}
	}
	virtual void handleEndElement(const QStringRef &name, const QStringRef &uri)
	{
		Q_UNUSED(name);
		Q_UNUSED(uri);
		if (m_depth == 2)
			m_atValue = 0;
		m_depth--;
	}
	virtual void handleCharacterData(const QStringRef &text)
	{
		if(m_depth == 2 && m_atValue)
			m_value = text.toString();
	}
	virtual void serialize(const QPair<QString, QString> *option, QXmlStreamWriter *writer)
	{
		writer->writeStartElement(QLatin1String("option"));
		writeAttribute(writer,QLatin1String("label"), option->first);
		writeTextElement(writer,QLatin1String("value"), option->second);
		writer->writeEndElement();
	}
	void serialize(const QList<QPair<QString, QString> > &options, QXmlStreamWriter *writer)
	{
		for (int i = 0; i < options.size(); i++)
			serialize(&options.at(i), writer);
	}
	const QPair<QString, QString> create()
	{
		return qMakePair(m_label, m_value);
	}
private:
	void clear()
	{
		m_label.clear();
		m_value.clear();
		m_atValue = 0;
	}
	int m_depth : 31;
	int m_atValue : 1;
	QString m_label;
	QString m_value;
};

static const char* datafield_types[] = {
	"boolean",
	"fixed",
	"hidden",
	"jid-multi",
	"jid-single",
	"list-multi",
	"list-single",
	"text-multi",
	"text-private",
	"text-single"
};

class DataFormFieldParser : XmlStreamFactory<DataFormField>
{
public:
	DataFormFieldParser()
	{
		m_depth = 0;
		m_state = AtNowhere;
	}
	virtual ~DataFormFieldParser()
	{

	}
	virtual bool canParse(const QStringRef &name, const QStringRef &uri,
						  const QXmlStreamAttributes &attributes)
	{
		Q_UNUSED(uri);
		Q_UNUSED(attributes);
		return name == QLatin1String("field");
	}
	virtual void handleStartElement(const QStringRef &name, const QStringRef &uri,
									const QXmlStreamAttributes &attributes)
	{
		m_depth++;
		if(m_depth == 1) {
			m_type = strToEnum<DataFormField::Type>(attributes.value(QLatin1String("type")),datafield_types);
			m_var = attributes.value(QLatin1String("var")).toString();
			m_label = attributes.value(QLatin1String("label")).toString();
		} else if (m_depth == 2) {
			if(name == QLatin1String("value")) {
				m_state = AtValue;
			} else if(m_optionParser.canParse(name,uri,attributes)) {
				m_state = AtOption;
			} else if(name == QLatin1String("required")) {
				m_state = AtRequied;
				m_required = true;
			} else {
				m_state = AtNowhere;
			}
		}
		if(m_state == AtOption)
			m_optionParser.handleStartElement(name,uri,attributes);
	}
	virtual void handleEndElement(const QStringRef &name, const QStringRef &uri)
	{
		if(m_state == AtOption) {
			m_optionParser.handleEndElement(name,uri);
			if(m_depth == 2) {
				qDebug() << m_optionParser.create();
				m_options.append(m_optionParser.create());
			}
		}
		if (m_depth <= 2)
			m_state = AtNowhere;
		m_depth--;
	}
	virtual void handleCharacterData(const QStringRef &text)
	{
		switch(m_state) {
		case AtValue:
			m_values.append(text.toString());
			break;
		case AtOption:
			m_optionParser.handleCharacterData(text);
		default:
			break;
		}
	}
	virtual void serialize(DataFormField *field, QXmlStreamWriter *writer)
	{
		writer->writeStartElement(QLatin1String("field"));
		writeAttribute(writer,QLatin1String("type"),enumToStr(field->type(),datafield_types));
		writeAttribute(writer,QLatin1String("label"),field->description());
		writeAttribute(writer,QLatin1String("var"),field->var());
		m_optionParser.serialize(DataFormFieldPrivate::get(field)->options, writer);
		foreach(const QVariant &value, field->values())
			writeTextElement(writer,QLatin1String("value"), value.toString());
		if(field->isRequired())
			writer->writeEmptyElement(QLatin1String("required"));
		writer->writeEndElement();
	}
	void serialize(const DataFormFieldContainer &fields, QXmlStreamWriter *writer)
	{
		for (int i = 0; i < fields.fieldsCount(); i++) {
			DataFormField field = fields.field(i);
			serialize(&field, writer);
		}
	}
	DataFormField create()
	{
		DataFormField field;
		DataFormFieldPrivate *d = DataFormFieldPrivate::get(&field);
		d->var = m_var;
		d->values = m_values;
		d->label = m_label;
		d->type = m_type;
		d->options = m_options;
//		qDebug() << m_label << m_var << field.var() << field.label() << d->var << d->label;
		d->required = m_required;
		clear();
		return field;
	}
private:
	enum State {
		AtValue,
		AtOption,
		AtRequied,
		AtNowhere
	};
	void clear() {
		m_options.clear();
		m_type = DataFormField::Invalid;
		m_label.clear();
		m_var.clear();
		m_values.clear();
		m_required = false;
		m_state = AtNowhere;
	}
	State m_state;
	int m_depth;
	DataFormField::Type m_type;
	QString m_label;
	QString m_var;
	QStringList m_values;
	QList<QPair<QString, QString> > m_options;
	bool m_required;
	DataFormOptionParser m_optionParser;
	MultimediaDataFactory m_multimediaDataFactory;
};

enum DataFormState { AtNowhere, AtTitle, AtInstruction, AtField };

//static const char* dataform_states[] = {
//	"title",
//	"instruction",
//	"field"
//};

static const char* dataform_types[] = {
	"form",
	"submit",
	"cancel",
	"result"
};

class DataFormFactoryPrivate
{
public:
	void clear()
	{
		title.clear();
		instruction.clear();
		fields.clear();
		formType = DataForm::Invalid;
	}
	int depth;
	DataFormState state;
	QString title;
	QString instruction;
	//DataFormReportedList reported;
	DataFormFieldList fields;
	DataForm::Type formType;
	DataFormFieldParser fieldParser;
};

DataFormFactory::DataFormFactory() : d_ptr(new DataFormFactoryPrivate)
{
	Q_D(DataFormFactory);
	d->depth = 0;
	d->clear();
}

DataFormFactory::~DataFormFactory()
{
}

QStringList DataFormFactory::features() const
{
	return QStringList(NS_DATAFORM);
}

bool DataFormFactory::canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(attributes);
	return name == QLatin1String("x") && uri == QLatin1String(NS_DATAFORM);
}

void DataFormFactory::handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_D(DataFormFactory);
	d->depth++;
	if(d->depth == 1) {
		d->formType = strToEnum<DataForm::Type>(attributes.value(QLatin1String("type")), dataform_types);
	} else if(d->depth == 2) {
		if(d->fieldParser.canParse(name, uri, attributes))
			d->state = AtField;
		else if(name == QLatin1String("title"))
			d->state = AtTitle;
		else if(name == QLatin1String("instruction"))
			d->state = AtInstruction;
		else
			d->state = AtNowhere;
	}
	if(d->state == AtField)
		d->fieldParser.handleStartElement(name,uri,attributes);
}

void DataFormFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	Q_D(DataFormFactory);
	if(d->state == AtField) {
		d->fieldParser.handleEndElement(name, uri);
		if(d->depth == 2) {
			d->fields.append(d->fieldParser.create());
			d->state = AtNowhere;
		}
	} else if(d->depth == 2) {
		d->state = AtNowhere;
	}
	d->depth--;
}

void DataFormFactory::handleCharacterData(const QStringRef &text)
{
	Q_D(DataFormFactory);
	switch(d->state) {
	case AtTitle:
		d->title = text.toString();
	case AtInstruction:
		d->instruction = text.toString();
	case AtField:
		d->fieldParser.handleCharacterData(text);
	default:
		break;
	}
}

void DataFormFactory::serialize(Payload *extension, QXmlStreamWriter *writer)
{
	Q_D(DataFormFactory);
	DataForm *form = se_cast<DataForm*>(extension);
	writer->writeStartElement(QLatin1String("x"));
	writer->writeDefaultNamespace(NS_DATAFORM);
	if (form->type() != DataForm::Invalid)
		writer->writeAttribute(QLatin1String("type"), enumToStr(form->type(), dataform_types));
	writeTextElement(writer,QLatin1String("title"), form->title());
//	writer->writeTextElement(QLatin1String("instruction"), form->));
	d->fieldParser.serialize(*form, writer);
	writer->writeEndElement();
}

Payload::Ptr DataFormFactory::createPayload()
{
	Q_D(DataFormFactory);
	DataForm *form = new DataForm(d->formType,d->title);
	form->setFields(d->fields);
	d->clear();
	return Payload::Ptr(form);
}

} // namespace Jreen
