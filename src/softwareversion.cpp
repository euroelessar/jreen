#include "softwareversion.h"

namespace Jreen
{

SoftwareVersion::SoftwareVersion(const QString &name, const QString &version, const QString &os)
{
	m_name = name;
	m_version = version;
	m_os = os;
}

} //namespace Jreen

