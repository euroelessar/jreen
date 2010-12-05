/****************************************************************************
 *  dataform.h
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
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

#ifndef DATAFORM_H
#define DATAFORM_H

#include "jid.h"
#include "stanzaextension.h"
#include <QStringList>
#include <QHash>

namespace jreen
{

// XEP-0004
// http://xmpp.org/extensions/xep-0004.html
struct JREEN_EXPORT DataFormOption
{
	QString label;
	QString value;
};

typedef QSharedPointer<DataFormOption> DataFormOptionPointer;
typedef QList<DataFormOptionPointer> DataFormOptionList;

class JREEN_EXPORT DataFormField
{
public:
	enum Type
	{
		Boolean,		/**< The field enables an entity to gather or provide an either-or
		* choice between two options. The default value is "false". */
		Fixed,			/**<  The field is intended for data description (e.g., human-readable
		* text such as "section" headers) rather than data gathering or
		* provision. The <value/> child SHOULD NOT contain newlines
		* (the \n and \r characters); instead an application SHOULD
		* generate multiple fixed fields, each with one <value/> child. */
		Hidden,			/**<  The field is not shown to the form-submitting entity, but
		* instead is returned with the form. The form-submitting entity
		* SHOULD NOT modify the value of a hidden field, but MAY do so
		* if such behavior is defined for the "using protocol". */
		JidMulti,		/**<  The field enables an entity to gather or provide multiple
		* Jabber IDs. Each provided JID SHOULD be unique (as determined
		* by comparison that includes application of the Nodeprep, Nameprep,
		* and Resourceprep profiles of Stringprep as specified in XMPP Core),
		* and duplicate JIDs MUST be ignored. */
		JidSingle,		/**<  The field enables an entity to gather or provide a single
		* Jabber ID. */
		ListMulti,		/**<  The field enables an entity to gather or provide one or more
		* options from among many. A form-submitting entity chooses one
		* or more items from among the options presented by the form-processing
		* entity and MUST NOT insert new options. The form-submitting entity
		* MUST NOT modify the order of items as received from the form-processing
		* entity, since the order of items MAY be significant. */
		ListSingle,		/**<  The field enables an entity to gather or provide one option
		* from among many. A form-submitting entity chooses one item
		* from among the options presented by the form-processing entity
		* and MUST NOT insert new options. */
		TextMulti,		/**<  The field enables an entity to gather or provide multiple
		* lines of text. */
		TextPrivate,	/**<  The field enables an entity to gather or provide a single
		* line or word of text, which shall be obscured in an interface
		* (e.g., with multiple instances of the asterisk character). */
		TextSingle,		/**<  The field enables an entity to gather or provide a single
		* line or word of text, which may be shown in an interface.
		* This field type is the default and MUST be assumed if a form-submitting
		* entity receives a field type it does not understand. */
		None,
		Invalid
	};
	inline DataFormField(Type type = TextSingle) : m_type(type), m_required(false) {}
	inline DataFormField(const QString &var, const QString &value, const QString &label = QString(), Type type = TextSingle)
		: m_type(type), m_var(var), m_label(label), m_values(QStringList()<<value), m_required(false) {}
	inline DataFormField(const QString &var, const QStringList &values, const QString &label = QString(), Type type = TextSingle)
		: m_type(type), m_var(var), m_label(label), m_values(values), m_required(false) {}
	inline DataFormField(const QString &var, const QString &label, Type type)
		: m_type(type), m_var(var), m_label(label), m_required(false) {}
	//	inline ~DataFormField() {}
	inline Type type() const { return m_type; }
	inline bool isValid() const { return m_type != Invalid; }
	inline const DataFormOptionList &options() const { return m_options; }
	inline DataFormOptionList &options() { return m_options; }
	inline void setOptions(const DataFormOptionList &options) { m_options = options; }
	inline QStringList &values() { return m_values; }
	inline QString value() {return m_values.size() ? m_values.first() : QString();}
	inline const QStringList &values() const { return m_values; }
	inline void setValues(const QStringList &values) { m_values = values; }
	inline const QString &var() const { return m_var; }
	inline void setVar(const QString &var) { m_var = var; }
	inline bool required() const { return m_required; }
	inline void setRequired(bool required) { m_required = required; }
	inline const QString &desc() const { return m_desc; }
	inline void setDesc(const QString &desc) { m_desc = desc; }
protected:
	Type m_type;
	QString m_var;
	QString m_desc;
	QString m_label;
	QStringList m_values;
	bool m_required;
	DataFormOptionList m_options;
};

//class DataFormFieldBoolean : public DataFormField
//{
//public:
//	inline DataFormFieldBoolean(const QString &var, bool value = false, const QString &label = QString())
//			: DataFormField(var,label,Boolean) { m_values = QStringList() << QLatin1String(value?"1":"0"); }
//	inline DataFormFieldBoolean(const QDomElement &node) : DataFormField(node) {}
//	inline bool value() const { return m_values.at(0) == QLatin1String("1") || m_values.at(0) == QLatin1String("true"); }
//	inline void setValue(bool value) { m_values[0] = QLatin1String(value ? "1" : "0"); }
//};
//
//class DataFormFieldFixed : public DataFormField
//{
//public:
//	inline DataFormFieldFixed(const QString &var, const QString &value = QString(), const QString &label = QString())
//			: DataFormField(var,label,Fixed) { m_values = QStringList() << value; }
//	inline DataFormFieldFixed(const QDomElement &node) : DataFormField(node) {}
//	inline const QString &value() const { return m_values.at(0); }
//	inline void setValue(const QString &value) { m_values[0] = value; }
//};
//
//class DataFormFieldHidden : public DataFormField
//{
//public:
//	inline DataFormFieldHidden(const QString &var, const QString &value = QString())
//			: DataFormField(var,QString(),Hidden) { m_values = QStringList() << value; }
//	inline DataFormFieldHidden(const QDomElement &node) : DataFormField(node) {}
//	inline const QString &value() const { return m_values.at(0); }
//	inline void setValue(const QString &value) { m_values[0] = value; }
//};
//
////JidMulti
//
//class DataFormFieldJidSingle : public DataFormField
//{
//public:
//	inline DataFormFieldJidSingle(const QString &var, const JID &value = JID(), const QString &label = QString())
//			: DataFormField(var,label,JidSingle) { m_values = QStringList() << value; }
//	inline DataFormFieldJidSingle(const QDomElement &node) : DataFormField(node) {}
//	inline JID value() const { return m_values.at(0); }
//	inline void setValue(const JID &value) { m_values[0] = value; }
//	inline void setValue(const QString &value) { m_values[0] = value; }
//};
//
////ListMulti
////ListSingle
////TextMulti
//
//class DataFormFieldTextPrivate : public DataFormField
//{
//public:
//	inline DataFormFieldTextPrivate(const QString &var, const QString &value = QString(), const QString &label = QString())
//			: DataFormField(var,label,TextPrivate) { m_values = QStringList() << value; }
//	inline DataFormFieldTextPrivate(const QDomElement &node) : DataFormField(node) {}
//	inline const QString &value() const { return m_values.at(0); }
//	inline void setValue(const QString &value) { m_values[0] = value; }
//};
//
//class DataFormFieldTextSingle : public DataFormField
//{
//public:
//	inline DataFormFieldTextSingle(const QString &var, const QString &value = QString(), const QString &label = QString())
//			: DataFormField(var,label,TextSingle) { m_values = QStringList() << value; }
//	inline DataFormFieldTextSingle(const QDomElement &node) : DataFormField(node) {}
//	inline const QString &value() const { return m_values.at(0); }
//	inline void setValue(const QString &value) { m_values[0] = value; }
//};
//
//class DataFormFieldNone : public DataFormField
//{
//public:
//	inline DataFormFieldNone(const QString &var, const QString &value = QString(), const QString &label = QString())
//			: DataFormField(var,label,None) { m_values = QStringList() << value; }
//	inline DataFormFieldNone(const QDomElement &node) : DataFormField(node) {}
//	inline const QString &value() const { return m_values.at(0); }
//	inline void setValue(const QString &value) { m_values[0] = value; }
//};

typedef QSharedPointer<DataFormField> DataFormFieldPointer;
typedef QList<DataFormFieldPointer> DataFormFieldList;
typedef QHash<QString,DataFormFieldPointer> DataFormFieldHash;

class JREEN_EXPORT DataFormFieldContainer
{
public:
	const DataFormFieldList &fields() const { return m_fields_list; }
	DataFormFieldList &fields() { return m_fields_list; }
	DataFormFieldPointer field(const QString &var) const;
	inline void setFields(const DataFormFieldList &fields) { m_fields_list = fields; }
	inline void appendField(DataFormFieldPointer field) {m_fields_list.append(field);}
	//	inline void appendField(DataFormField *field);
	//	inline QSharedPointer<DataFormField> appendField(DataFormField *field);
	//	QSharedPointer<DataFormField> addField(DataFormField::Type,
protected:
	DataFormFieldList m_fields_list;
	DataFormFieldHash m_fields_hash;
};

class JREEN_EXPORT DataFormItem : public DataFormFieldContainer
{
public:
};

typedef QList<QSharedPointer<DataFormItem> > DataFormItemList;

class JREEN_EXPORT DataFormReported : public DataFormFieldContainer
{
public:
};

typedef QList<QSharedPointer<DataFormReported> > DataFormReportedList;

class JREEN_EXPORT DataForm : public StanzaExtension, public DataFormFieldContainer
{
	J_EXTENSION(jreen::DataForm,"/message/x[@xmlns='jabber:x:data']")
public:
	enum Type
	{
		Form,    // The form-processing entity is asking the form-submitting entity to complete a form.
		Submit,  // The form-submitting entity is submitting data to the form-processing entity. The submission MAY include fields that were not provided in the empty form, but the form-processing entity MUST ignore any fields that it does not understand.
		Cancel,  // The form-submitting entity has cancelled submission of data to the form-processing entity.
		Result,  // The form-processing entity is returning data (e.g., search results) to the form-submitting entity, or the data is a generic data set.
		Invalid
	};
	DataForm(Type type,const QString &title = QString());
	const QString &title() { return m_title; }
private:
	QString m_title;
	DataFormReportedList m_reported;
	DataFormItemList m_items;
	Type m_form_type;
};

}

#endif // DATAFORM_H
