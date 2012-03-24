/****************************************************************************
**
** Jreen
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef DATAFORM_H
#define DATAFORM_H

#include "jid.h"
#include "stanzaextension.h"
#include <QStringList>
#include <QHash>
#include <QVariant>

namespace Jreen
{

// XEP-0004
// http://xmpp.org/extensions/xep-0004.html

class DataFormFieldPrivate;
class DataFormMediaPrivate;
class Stanza;

class JREEN_EXPORT DataFormMedia
{
	Q_DECLARE_PRIVATE(DataFormMedia)
public:
	typedef QSharedPointer<DataFormMedia> Ptr;
	
	class UriPrivate;
	class JREEN_EXPORT Uri
	{
	public:
		Uri();
		Uri(const QUrl &url, const QString &type);
		Uri(const Uri &o);
		Uri &operator =(const Uri &o);
		~Uri();
		
		bool operator==(const Uri &o) const;
		
		QUrl url() const;
		void setUrl(const QUrl &url);
		QString type() const;
		void setType(const QString &type);
	private:
		QSharedDataPointer<UriPrivate> d;
	};
	
	DataFormMedia();
	~DataFormMedia();
	
	void appendUri(const Uri &uri);
	void appendUri(const QUrl &url, const QString &type);
	void setUries(const QList<Uri> &uries);
	QList<Uri> uries() const;
	
	QSize size() const;
	void setSize(const QSize &size);
	int width() const;
	void setWidth(int width);
	int height() const;
	void setHeight(int height);
	
private:
	Q_DISABLE_COPY(DataFormMedia)
	QScopedPointer<DataFormMediaPrivate> d_ptr;
};

class JREEN_EXPORT DataFormField
{
public:
	enum Type
	{
		Invalid = -1,
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
		None
	};
	
	DataFormField(Type type = Invalid, const QString &var = QString(), const QString &label = QString());
	DataFormField(const DataFormField &that);
	DataFormField &operator =(const DataFormField &that);
	~DataFormField();
	
	Type type() const;
	bool isRequired() const;
	void setRequired(bool required);
	QString var() const;
	void setVar(const QString &var);
	QString label() const;
	void setLabel(const QString &label);
	QString description() const;
	void setDescription(const QString &desc);
	
	DataFormMedia::Ptr media() const;
	void setMedia(const DataFormMedia::Ptr &media);
	
	void setValues(const QStringList &values);
	QStringList values() const;
	void setValue(const QString &value);
	QString value() const;
	
	template <typename T> T cast() const
	{ return T(static_cast<DataFormField::Type>(T::StaticType) == type() ? *this : DataFormField()); }

protected:
	QExplicitlySharedDataPointer<DataFormFieldPrivate> d_ptr;
	friend class DataFormFieldPrivate;
};

class JREEN_EXPORT DataFormFieldBoolean : public DataFormField
{
public:
	enum { StaticType = DataFormField::Boolean };
	DataFormFieldBoolean(const QString &var, bool value = false, const QString &label = QString());

	bool value() const;
	void setValue(bool value);
protected:
	DataFormFieldBoolean(const DataFormField &o);
	friend class DataFormField;
};

class JREEN_EXPORT DataFormFieldFixed : public DataFormField
{
public:
	enum { StaticType = DataFormField::Fixed };
	DataFormFieldFixed(const QString &var, const QString &value = QString(), const QString &label = QString());

	QString value() const;
	void setValue(const QString &value);
protected:
	DataFormFieldFixed(const DataFormField &o);
	friend class DataFormField;
};

class JREEN_EXPORT DataFormFieldHidden : public DataFormField
{
public:
	enum { StaticType = DataFormField::Hidden };
	DataFormFieldHidden(const QString &var, const QString &value = QString());

	QString value() const;
	void setValue(const QString &value);
protected:
	DataFormFieldHidden(const DataFormField &o);
	friend class DataFormField;
};

class JREEN_EXPORT DataFormFieldJidMulti : public DataFormField
{
public:
	enum { StaticType = DataFormField::JidMulti };
	DataFormFieldJidMulti(const QString &var, const QList<JID> &values = QList<JID>(), const QString &label = QString());

	QList<JID> values() const;
	void setValues(const QList<JID> &values);
protected:
	DataFormFieldJidMulti(const DataFormField &o);
	friend class DataFormField;
};

class JREEN_EXPORT DataFormFieldJidSingle : public DataFormField
{
public:
	enum { StaticType = DataFormField::JidSingle };
	DataFormFieldJidSingle(const QString &var, const JID &value = JID(), const QString &label = QString());

	JID value() const;
	void setValue(const JID &value);
protected:
	DataFormFieldJidSingle(const DataFormField &o);
	friend class DataFormField;
};

class JREEN_EXPORT DataFormOptionContainer : public DataFormField
{
public:
	int optionsCount() const;
	QString optionLabel(int index) const;
	QString optionValue(int index) const;
	void addOption(const QString &label, const QString &value);
	void removeOption(int index);
protected:
	DataFormOptionContainer(const DataFormField &that);
	DataFormOptionContainer(Type type = Invalid, const QString &var = QString(), const QString &label = QString());
	friend class DataFormField;
};

class JREEN_EXPORT DataFormFieldListMulti : public DataFormOptionContainer
{
public:
	enum { StaticType = DataFormField::ListMulti };
	DataFormFieldListMulti(const QString &var, const QStringList &values = QStringList(), const QString &label = QString());

	bool isChecked(int index) const;
	void setChecked(int index, bool checked);
protected:
	DataFormFieldListMulti(const DataFormField &o);
	friend class DataFormField;
};

class JREEN_EXPORT DataFormFieldListSingle : public DataFormOptionContainer
{
public:
	enum { StaticType = DataFormField::ListSingle };
	DataFormFieldListSingle(const QString &var, const QString &value = QString(), const QString &label = QString());

	QString value() const;
	void setValue(const QString &value);
protected:
	DataFormFieldListSingle(const DataFormField &o);
	friend class DataFormField;
};

class JREEN_EXPORT DataFormFieldTextMulti : public DataFormField
{
public:
	enum { StaticType = DataFormField::TextMulti };
	DataFormFieldTextMulti(const QString &var, const QString &value = QString(), const QString &label = QString());

	QString value() const;
	void setValue(const QString &value);
protected:
	DataFormFieldTextMulti(const DataFormField &o);
	friend class DataFormField;
};

class JREEN_EXPORT DataFormFieldTextPrivate : public DataFormField
{
public:
	enum { StaticType = DataFormField::TextPrivate };
	DataFormFieldTextPrivate(const QString &var, const QString &value = QString(), const QString &label = QString());

	QString value() const;
	void setValue(const QString &value);
protected:
	DataFormFieldTextPrivate(const DataFormField &o);
	friend class DataFormField;
};

class JREEN_EXPORT DataFormFieldTextSingle : public DataFormField
{
public:
	enum { StaticType = DataFormField::TextSingle };
	DataFormFieldTextSingle(const QString &var, const QString &value = QString(), const QString &label = QString());

	QString value() const;
	void setValue(const QString &value);
protected:
	DataFormFieldTextSingle(const DataFormField &o);
	friend class DataFormField;
};

class JREEN_EXPORT DataFormFieldNone : public DataFormField
{
public:
	enum { StaticType = DataFormField::None };
	DataFormFieldNone(const QString &var, const QStringList &values = QStringList(), const QString &label = QString());

	QStringList values() const;
	void setValues(const QStringList &values);
protected:
	DataFormFieldNone(const DataFormField &o);
	friend class DataFormField;
};

typedef QSharedPointer<DataFormField> DataFormFieldPointer;
typedef QList<DataFormField> DataFormFieldList;
typedef QHash<QString, DataFormField> DataFormFieldHash;

class DataFormFieldContainerPrivate;

class JREEN_EXPORT DataFormFieldContainer
{
	Q_DECLARE_PRIVATE(DataFormFieldContainer)
	Q_DISABLE_COPY(DataFormFieldContainer)
public:
	int fieldsCount() const;
	DataFormField field(int index) const;
	DataFormField field(const QString &var) const;
	DataFormField field(const QLatin1String &var) const;
	
	void removeField(int index);
	void appendField(const DataFormField &field);
	void setFields(const QList<DataFormField> &fields);
protected:
	DataFormFieldContainer();
	DataFormFieldContainer(DataFormFieldContainerPrivate &d);
	virtual ~DataFormFieldContainer();

	QScopedPointer<DataFormFieldContainerPrivate> d_ptr;
};

class JREEN_EXPORT DataFormItem : public DataFormFieldContainer
{
public:
	typedef QSharedPointer<DataFormItem> Ptr;
};

typedef QList<QSharedPointer<DataFormItem> > DataFormItemList;

class JREEN_EXPORT DataFormReported : public DataFormFieldContainer
{
public:
	typedef QSharedPointer<DataFormReported> Ptr;
};

typedef QList<QSharedPointer<DataFormReported> > DataFormReportedList;

class DataFormPrivate;
class JREEN_EXPORT DataForm : public Payload, public DataFormFieldContainer
{
	J_PAYLOAD(Jreen::DataForm)
	Q_DECLARE_PRIVATE(DataForm)
public:
	enum Type
	{
		Form,    // The form-processing entity is asking the form-submitting entity to complete a form.
		Submit,  // The form-submitting entity is submitting data to the form-processing entity. The submission MAY include fields that were not provided in the empty form, but the form-processing entity MUST ignore any fields that it does not understand.
		Cancel,  // The form-submitting entity has cancelled submission of data to the form-processing entity.
		Result,  // The form-processing entity is returning data (e.g., search results) to the form-submitting entity, or the data is a generic data set.
		Invalid
	};
	DataForm(Type type = Submit, const QString &title = QString());
	DataForm(Type type, const QString &title, const QString &instructions);
	virtual ~DataForm();
	
	Type type() const;
	void setType(Type type);
	QString typeName() const;
	void setTypeName(const QString &type);
	QString title() const;
	QString instructions() const;
	QList<DataFormItem::Ptr> items() const;
	DataFormReported::Ptr reported() const;
};

template <>
Q_INLINE_TEMPLATE DataFormOptionContainer DataFormField::cast() const
{
	bool ok = type() == ListMulti || type() == ListSingle;
	return DataFormOptionContainer(ok ? *this : DataFormField());
}

}

#endif // DATAFORM_H
