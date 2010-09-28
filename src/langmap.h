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
#include "jreen.h"

J_BEGIN_NAMESPACE

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

	void fillNode( QDomElement &node, const QString &name, const QString &uri ) const;
	const QString &value( const QString &lang ) const;
	const QString &value( const QString &lang, const QString &default_value ) const;

	static LangMapData shared_null;
};


class LangMap
{
	LangMapData *j;
public:
	inline LangMap() : j(&LangMapData::shared_null) { j->ref.ref(); }
	inline LangMap( const QString &value ) : j(new LangMapData) { j->ref = 1; j->base = value; }
	inline LangMap( const LangMap &other ) : j(other.j) { j->ref.ref(); }
	inline ~LangMap() { if( !j->ref.deref() ) delete j; }

	LangMap &operator =( const LangMap &other )
	{
		if( j != other.j )
		{
			other.j->ref.ref();
			if( !j->ref.deref() )
				delete j;
			j = other.j;
		}
		return *this;
	}
	inline bool operator ==( const LangMap &other ) const;

	inline void clear();

	inline bool contains( const QString &lang ) const;
	inline void insert( const QString &lang, const QString &value );

	inline const QString lang( const QString &value ) const;
	inline const QString lang( const QString &value, const QString &default_lang ) const;
	inline const QString &value( const QString &lang = QString() ) const;
	inline const QString &value( const QString &lang, const QString &default_value ) const;

	inline QString &operator[]( const QString &lang );
	inline const QString operator[]( const QString &lang ) const;

	inline QStringList langs() const;
	inline QStringList langs( const QString &value ) const;
	inline QStringList values() const;
	inline QStringList values( const QString &lang = QString() ) const;

	inline void fillNode( QDomElement &elem, const QString &name, const QString &uri = QString() ) const;

	inline int count() const;
	inline int size() const { return count(); }
	inline bool isEmpty() const { return count() == 0; }
protected:
	inline void detach() { if( j->ref != 1 ) detach_helper(); }
	void detach_helper()
	{
		j->ref.deref();
		LangMapData *d = j;
		j = new LangMapData;
		j->ref = 1;
		j->base = d->base;
		j->other = d->other;
	}
};

inline bool LangMap::operator ==( const LangMap &other ) const
{
	return j == other.j || ( j->base == other.j->base && j->other == other.j->other );
}

inline void LangMap::clear()
{
	*this = LangMap();
}

inline bool LangMap::contains( const QString &lang ) const
{
	return lang.isEmpty() ? !j->base.isEmpty() : j->other.contains( lang );
}

inline void LangMap::insert( const QString &lang, const QString &value )
{
	detach(); ( lang.isEmpty() ? j->base : j->other[lang] ) = value;
}

inline const QString LangMap::lang(const QString &value) const
{
	return value == j->base ? QString() : j->other.key( value );
}

inline const QString LangMap::lang(const QString &value, const QString &default_lang ) const
{
	return value == j->base ? QString() : j->other.key( value, default_lang );
}

inline const QString &LangMap::value(const QString &lang ) const
{
	return j->value( lang );
}

inline const QString &LangMap::value(const QString &lang, const QString &default_value) const
{
	return j->value( lang, default_value );
}

inline QString &LangMap::operator[](const QString &lang )
{
	detach(); return lang.isEmpty() ? j->base : j->other.operator []( lang );
}

inline const QString LangMap::operator[](const QString &lang ) const
{
	return lang.isEmpty() ? j->base : j->other.operator []( lang );
}

inline QStringList LangMap::langs() const
{
	return j->base.isEmpty() ? j->other.keys() : ( j->other.keys() << QString() );
}

inline QStringList LangMap::langs(const QString &value) const
{
	return ( j->base.isEmpty() || j->base != value ) ? j->other.keys( value ) : ( j->other.keys( value ) << QString() );
}

inline QStringList LangMap::values() const
{
	return j->base.isEmpty() ? j->other.values() : ( j->other.values() << j->base );
}

inline QStringList LangMap::values(const QString &lang ) const
{
	return j->base.isEmpty() ? j->other.values( lang ) : ( j->other.values() << j->base );
}

inline void LangMap::fillNode( QDomElement &node, const QString &name, const QString &uri ) const
{
	j->fillNode( node, name, uri );
}

inline int LangMap::count() const
{
	return j->other.count() + ( j->base.isEmpty() ? 0 : 1 );
}

J_END_NAMESPACE

#endif // LANGMAP_H
