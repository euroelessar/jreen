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

#include "jid.h"
#include "prep_p.h"
#include <QAtomicInt>
#include <QStringBuilder>

namespace Jreen
{

class JIDData : public QSharedData
{
public:
	JIDData()
	{
		valid = false;
	}
	JIDData(const JIDData &other) :
		QSharedData(other), domain(other.domain),
		node(other.node), resource(other.resource),
		bare(other.bare), full(other.full),
		valid(other.valid)
	{

	}

	inline void setStrings() { setBare(); setFull(); }

	void setBare()
	{
		if(!node.isEmpty())
			this->bare = node + QLatin1Char('@');
		else
			this->bare.clear();
		this->bare += domain;
	}

	void setFull()
	{
		this->full = bare;
		if(!resource.isEmpty())
			this->full += QLatin1Char('/') + resource;
	}

	QString domain;
	QString node;
	QString resource;
	QString bare;
	QString full;
	bool valid;
};

JID::JID() : d_ptr(new JIDData)
{
}

JID::JID(const JID &jid) : d_ptr(jid.d_ptr)
{
}

JID::JID(const QString &jid) : d_ptr(new JIDData)
{
	setJID(jid);
}

JID::JID(const QLatin1String &jid) : d_ptr(new JIDData)
{
	setJID(jid);
}

JID::JID(const QString &node, const QString &domain, const QString &resource) :
	d_ptr(new JIDData)
{
	d_ptr->node = Prep::instance().nodePrep(node, &d_ptr->valid);
	if(!d_ptr->valid)
		return;
	d_ptr->domain = Prep::instance().namePrep(domain, &d_ptr->valid);
	if(!d_ptr->valid)
		return;
	d_ptr->resource = Prep::instance().resourcePrep(resource, &d_ptr->valid);
	if(!d_ptr->valid)
		return;
	d_ptr->setStrings();
}

JID::~JID()
{
}

bool JID::setJID(const QString &jid)
{
	if(jid.isEmpty())
		return false;

	d_ptr->node.clear();
	d_ptr->domain.clear();
	d_ptr->resource.clear();

	int at = jid.indexOf(QLatin1Char('@'));
	int slash = jid.indexOf(QLatin1Char('/'), at < 0 ? 0 : at);

	if(at > -1) {
		d_ptr->node = Prep::instance().nodePrep(jid.mid(0, at), &d_ptr->valid);
		if(!d_ptr->valid)
			return false;
	}
	d_ptr->domain = Prep::instance().namePrep(jid.mid(at < 0 ? 0 : at + 1, slash - at - 1), &d_ptr->valid);
	if(!d_ptr->valid)
		return false;

	if(slash > -1) {
		d_ptr->resource = Prep::instance().resourcePrep(jid.mid(slash + 1), &d_ptr->valid);
		if(!d_ptr->valid)
			return false;
	}

	d_ptr->setStrings();
	return d_ptr->valid;
}

const QString &JID::domain() const
{
	return d_ptr->domain;
}

const QString &JID::node() const
{
	return d_ptr->node;
}

const QString &JID::resource() const
{
	return d_ptr->resource;
}

const QString &JID::bare() const
{
	return d_ptr->bare;
}

const QString &JID::full() const
{
	return d_ptr->full;
}

JID JID::bareJID() const
{
	if (isBare() || isDomain())
		return *this;
	JID jid;
	JIDData *o = jid.d_ptr.data();
	const JIDData *d = d_ptr.data();
	o->valid = d->valid;
	if (o->valid) {
		o->node = d->node;
		o->domain = d->domain;
		o->bare = d->bare;
		o->full = d->bare;
	}
	return jid;
}

bool JID::setNode(const QString &node)
{
	d_ptr->node = Prep::instance().nodePrep(node, &d_ptr->valid);
	d_ptr->setStrings();
	return d_ptr->valid;
}

bool JID::setDomain(const QString &domain)
{
	d_ptr->domain = Prep::instance().namePrep(domain, &d_ptr->valid);
	d_ptr->setStrings();
	return d_ptr->valid;
}

bool JID::setResource(const QString &resource)
{
	d_ptr->resource = Prep::instance().resourcePrep(resource, &d_ptr->valid);
	d_ptr->setFull();
	return d_ptr->valid;
}

JID JID::withNode(const QString &node) const
{
	JID j = *this;
	j.setNode(node);
	return j;
}

JID JID::withResource(const QString &resource) const
{
	JID j = *this;
	j.setResource(resource);
	return j;
}

JID &JID::operator =(const JID &other)
{
	d_ptr = other.d_ptr;
	return *this;
}

bool JID::operator ==(const QString &right) const
{
	return d_ptr->full == right;
}

bool JID::operator !=(const QString &right) const
{
	return d_ptr->full != right;
}

bool JID::operator ==(const QLatin1String &right) const
{
	return d_ptr->full == right;
}

bool JID::operator !=(const QLatin1String &right) const
{
	return d_ptr->full != right;
}

bool JID::operator ==(const JID& right) const
{
	if(d_ptr == right.d_ptr)
		return true;
	if(d_ptr && right.d_ptr)
		return d_ptr->full == right.d_ptr->full;
	return false;
}

bool JID::operator !=(const JID& right) const
{
	if(d_ptr == right.d_ptr)
		return false;
	if(d_ptr && right.d_ptr)
		return d_ptr->full != right.d_ptr->full;
	return true;
}

JID &JID::operator =(const QString &s)
{
	setJID(s);
	return *this;
}

JID &JID::operator =(const QLatin1String &s)
{
	setJID(s);
	return *this;
}

bool JID::isValid() const
{
	return d_ptr ? d_ptr->valid : false;
}

JID::operator QString() const
{
	return d_ptr->full;
}

bool JID::isBare() const
{
	return isValid() && resource().isEmpty() && !node().isEmpty();
}

bool JID::isFull() const
{
	return isValid() && !node().isEmpty() && !resource().isEmpty();
}

bool JID::isDomain() const
{
	return isValid() && resource().isEmpty() && node().isEmpty();
}

}
