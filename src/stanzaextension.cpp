#include "stanzaextension.h"
#include <QHash>

namespace Jreen
{
typedef QHash<QByteArray, int> ByteArrayHash;
Q_GLOBAL_STATIC(ByteArrayHash, seClassHash)

StanzaExtension::StanzaExtension()
{
}

StanzaExtension::~StanzaExtension()
{
}

int StanzaExtension::registerExtensionType(const char *type)
{
	QByteArray t = type;
	int id = seClassHash()->value(t, seClassHash()->size());
	if (id == seClassHash()->size())
		seClassHash()->insert(t, id);
	return id;
}

AbstractStanzaExtensionFactory::AbstractStanzaExtensionFactory()
{
}

AbstractStanzaExtensionFactory::~AbstractStanzaExtensionFactory()
{
}
}
