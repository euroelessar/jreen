/****************************************************************************
 *  jid.cpp
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

#include "jid.h"
#include "prep.h"
#include <QAtomicInt>
#include <QStringBuilder>

namespace jreen
{

static const QString empty_string;

class JIDPrivate
{
public:
	JIDPrivate()
	{
		ref = 1;
		valid = false;
	}

	inline void setStrings() { setBare(); setFull(); }

	void setBare() {
		if(!node.isEmpty())
			bare = node + QLatin1Char('@');
		else
			bare.clear();
		bare += domain;
	}

	void setFull() {
		full = bare;
		if(!resource.isEmpty())
		  full += QLatin1Char('/') + resource;
	}

	QString domain;
	QString node;
	QString resource;
	QString bare;
	QString full;
	bool valid;
	QAtomicInt ref;
};

JID::JID()
{
	impl = 0;
}

JID::JID(const JID &jid)
{
	impl = 0;
	operator =(jid);
}

JID::JID(const QString &jid)
{
	impl = 0;
	operator =(jid);
}

JID::JID(const QLatin1String &jid)
{
	impl = 0;
	operator =(jid);
}

JID::JID(const QString &node, const QString &domain, const QString &resource)
{
	impl = new JIDPrivate;
	impl->node = Prep::instance().nodePrep(node, &impl->valid);
	if(!impl->valid)
		return;
	impl->domain = Prep::instance().namePrep(domain, &impl->valid);
	if(!impl->valid)
		return;
	impl->resource = Prep::instance().resourcePrep(resource, &impl->valid);
	if(!impl->valid)
		return;
	impl->setStrings();
}

JID::~JID()
{
	if(impl && !impl->ref.deref())
		delete impl;
}

bool JID::setJID(const QString &jid)
{
	if(impl && impl->ref != 1) {
		impl->ref.deref();
		impl = 0;
	}
	if(jid.isEmpty()) {
		if(impl && impl->ref.deref())
			delete impl;
		impl = 0;
		return false;
	}
	if(!impl)
		impl = new JIDPrivate;

	impl->node.clear();
	impl->domain.clear();
	impl->resource.clear();

	int at = jid.indexOf(QLatin1Char('@'));
	int slash = jid.indexOf(QLatin1Char('/'), at < 0 ? 0 : at);

	if(at > -1) {
		impl->node = Prep::instance().nodePrep(jid.mid(0, at), &impl->valid);
		if(!impl->valid)
			return false;
	}

	impl->domain = Prep::instance().namePrep(jid.mid(at < 0 ? 0 : at + 1, slash - at - 1), &impl->valid);
	if(!impl->valid)
		return false;

	if(slash > -1) {
		impl->resource = Prep::instance().resourcePrep(jid.mid(slash + 1), &impl->valid);
		if(!impl->valid)
			return false;
	}

	impl->setStrings();

	return impl->valid;
}

const QString &JID::domain() const
{
	return impl ? impl->domain : empty_string;
}

const QString &JID::node() const
{
	return impl ? impl->node : empty_string;
}

const QString &JID::resource() const
{
	return impl ? impl->resource : empty_string;
}

const QString &JID::bare() const
{
	return impl ? impl->bare : empty_string;
}

const QString &JID::full() const
{
	return impl ? impl->full : empty_string;
}

bool JID::setNode(const QString &node)
{
	if(impl && impl->ref != 1) {
		impl->ref.deref();
		QString &domain = impl->domain;
		QString &resource = impl->resource;
		impl = new JIDPrivate;
		impl->domain = domain;
		impl->resource = resource;
	}
	if(!impl)
		impl = new JIDPrivate;
	impl->node = Prep::instance().nodePrep(node, &impl->valid);
	impl->setStrings();
	return impl->valid;
}

bool JID::setDomain(const QString &domain)
{
	if(impl && impl->ref != 1) {
		impl->ref.deref();
		QString &node = impl->node;
		QString &resource = impl->resource;
		impl = new JIDPrivate;
		impl->node = node;
		impl->resource = resource;
	}
	if(!impl)
		impl = new JIDPrivate;
	impl->domain = Prep::instance().namePrep(domain, &impl->valid);
	impl->setStrings();
	return impl->valid;
}

bool JID::setResource(const QString &resource)
{
	if(impl && impl->ref != 1) {
		impl->ref.deref();
		QString &node = impl->node;
		QString &domain = impl->domain;
		QString &bare = impl->bare;
		impl = new JIDPrivate;
		impl->node = node;
		impl->domain = domain;
		impl->bare = bare;
	}
	if(!impl)
		impl = new JIDPrivate;
	impl->resource = Prep::instance().resourcePrep(resource, &impl->valid);
	impl->setFull();
	return impl->valid;
}

JID JID::clone() const
{
	JID j;
	j.impl = new JIDPrivate;
	j.impl->domain = impl->domain;
	j.impl->node = impl->node;
	j.impl->resource = impl->resource;
	j.impl->bare = impl->bare;
	j.impl->full = impl->full;
	j.impl->valid = impl->valid;
	return j;
}

JID JID::withNode(const QString &node) const
{
	JID j = this->clone();
	j.setNode(node);
	return j;
}

JID JID::withResource(const QString &resource) const
{
	JID j = this->clone();
	j.setResource(resource);
	return j;
}

JID &JID::operator =(const JID &jid)
{
	if(jid.impl)
		jid.impl->ref.ref();
	if(impl && !impl->ref.deref())
		delete impl;
	impl = jid.impl;
	return *this;
}

bool JID::operator ==(const QString &right) const
{
	if(!impl)
		return right.isEmpty();
	return impl->full == right;
}

bool JID::operator !=(const QString &right) const
{
	if(!impl)
		return !right.isEmpty();
	return impl->full != right;
}

bool JID::operator ==(const QLatin1String &right) const
{
	if(!impl)
		return (*right.latin1()) == '\0';
	return impl->full == right;
}

bool JID::operator !=(const QLatin1String &right) const
{
	if(!impl)
		return (*right.latin1()) != '\0';
	return impl->full != right;
}

bool JID::operator ==(const JID& right) const
{
	if(impl == right.impl)
		return true;
	if(impl && right.impl)
		return impl->full == right.impl->full;
	return false;
}

bool JID::operator !=(const JID& right) const
{
	if(impl == right.impl)
		return false;
	if(impl && right.impl)
		return impl->full != right.impl->full;
	return true;
}

bool JID::isValid() const
{
	return impl ? impl->valid : false;
}

JID::operator QString() const
{
	return impl ? impl->full : empty_string;
}

}
