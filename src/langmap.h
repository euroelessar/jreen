/****************************************************************************
 *  langmap.h
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

#ifndef LANGMAP_H
#define LANGMAP_H

#include <QStringList>
#ifdef LANGMAP_USE_MAP
# include <QMap>
#else
# include <QHash>
#endif
class QXmlStreamWriter;
#include "jreen.h"

namespace Jreen
{

struct JREEN_EXPORT LangMapData
{
#ifdef LANGMAP_USE_MAP
	typedef QMap<QString,QString>  Base;
#else
	typedef QHash<QString,QString> Base;
#endif
	QBasicAtomicInt ref;
	QString base;
	Base other;

	void fillNode(QXmlStreamWriter *writer, const QString &name, const QString &uri) const;
	const QString &value(const QString &lang) const;
	const QString &value(const QString &lang, const QString &default_value) const;

	static LangMapData shared_null;
};


class LangMap
{
	LangMapData *d;
public:
	inline LangMap() : d(&LangMapData::shared_null) { d->ref.ref(); }
	inline LangMap(const QString &value) : d(new LangMapData) { d->ref = 1; d->base = value; }
	inline LangMap(const LangMap &other) : d(other.d) { d->ref.ref(); }
	inline ~LangMap() { if(!d->ref.deref()) delete d; }

	LangMap &operator =(const LangMap &other)
	{
		if(d != other.d)
		{
			other.d->ref.ref();
			if(!d->ref.deref())
				delete d;
			d = other.d;
		}
		return *this;
	}
	inline bool operator ==(const LangMap &other) const;

	inline void clear();

	inline bool contains(const QString &lang) const;
	inline void insert(const QString &lang, const QString &value);

	inline const QString lang(const QString &value) const;
	inline const QString lang(const QString &value, const QString &default_lang) const;
	inline const QString &value(const QString &lang = QString()) const;
	inline const QString &value(const QString &lang, const QString &default_value) const;

	inline QString &operator[](const QString &lang);
	inline const QString operator[](const QString &lang) const;

	inline QStringList langs() const;
	inline QStringList langs(const QString &value) const;
	inline QStringList values() const;
	inline QStringList values(const QString &lang = QString()) const;

	inline void fillNode(QXmlStreamWriter *writer, const QString &name, const QString &uri = QString()) const;

	inline int count() const;
	inline int size() const { return count(); }
	inline bool isEmpty() const { return count() == 0; }
protected:
	inline void detach() { if(d->ref != 1) detach_helper(); }
	void detach_helper()
	{
		d->ref.deref();
		LangMapData *p = d;
		d = new LangMapData;
		d->ref = 1;
		d->base = p->base;
		d->other = p->other;
	}
};

inline bool LangMap::operator ==(const LangMap &other) const
{
	return d == other.d || (d->base == other.d->base && d->other == other.d->other);
}

inline void LangMap::clear()
{
	*this = LangMap();
}

inline bool LangMap::contains(const QString &lang) const
{
	return lang.isEmpty() ? !d->base.isEmpty() : d->other.contains(lang);
}

inline void LangMap::insert(const QString &lang, const QString &value)
{
	detach(); (lang.isEmpty() ? d->base : d->other[lang]) = value;
}

inline const QString LangMap::lang(const QString &value) const
{
	return value == d->base ? QString() : d->other.key(value);
}

inline const QString LangMap::lang(const QString &value, const QString &default_lang) const
{
	return value == d->base ? QString() : d->other.key(value, default_lang);
}

inline const QString &LangMap::value(const QString &lang) const
{
	return d->value(lang);
}

inline const QString &LangMap::value(const QString &lang, const QString &default_value) const
{
	return d->value(lang, default_value);
}

inline QString &LangMap::operator[](const QString &lang)
{
	detach(); return lang.isEmpty() ? d->base : d->other.operator [](lang);
}

inline const QString LangMap::operator[](const QString &lang) const
{
	return lang.isEmpty() ? d->base : d->other.operator [](lang);
}

inline QStringList LangMap::langs() const
{
	return d->base.isEmpty() ? d->other.keys() : (d->other.keys() << QString());
}

inline QStringList LangMap::langs(const QString &value) const
{
	return (d->base.isEmpty() || d->base != value) ? d->other.keys(value) : (d->other.keys(value) << QString());
}

inline QStringList LangMap::values() const
{
	return d->base.isEmpty() ? d->other.values() : (d->other.values() << d->base);
}

inline QStringList LangMap::values(const QString &lang) const
{
	return d->base.isEmpty() ? d->other.values(lang) : (d->other.values() << d->base);
}

inline void LangMap::fillNode(QXmlStreamWriter *writer, const QString &name, const QString &uri) const
{
	d->fillNode(writer, name, uri);
}

inline int LangMap::count() const
{
	return d->other.count() + (d->base.isEmpty() ? 0 : 1);
}

}

#endif // LANGMAP_H
