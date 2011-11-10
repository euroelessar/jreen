#include "softwareversion.h"

namespace Jreen
{

class SoftwareVersionPrivate
{
public:
	QString name;
	QString version;
	QString os;
};

SoftwareVersion::SoftwareVersion()
    : d_ptr(new SoftwareVersionPrivate)
{
}

SoftwareVersion::SoftwareVersion(const QString &name, const QString &version, const QString &os)
    : d_ptr(new SoftwareVersionPrivate)
{
	Q_D(SoftwareVersion);
	d->name = name;
	d->version = version;
	d->os = os;
}

SoftwareVersion::~SoftwareVersion()
{
}

QString SoftwareVersion::name() const
{
	return d_func()->name; 
}

QString SoftwareVersion::version() const
{
	return d_func()->version;
}

QString SoftwareVersion::os() const
{
	return d_func()->os;
}

} //namespace Jreen

