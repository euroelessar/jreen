#include "stanzaextension.h"
#include <QHash>

namespace Jreen
{
typedef QHash<QByteArray, int> ByteArrayHash;
Q_GLOBAL_STATIC(ByteArrayHash, seClassHash)
Q_GLOBAL_STATIC(QVector<QByteArray>, seClassVector)

Payload::Payload()
{
}

Payload::~Payload()
{
}

int Payload::registerPayloadType(const char *type)
{
	QByteArray t = type;
	int id = seClassHash()->value(t, seClassHash()->size());
	if (id == seClassHash()->size()) {
		ByteArrayHash::Iterator it = seClassHash()->insert(t, id);
		seClassVector()->append(it.key());
	}
	return id;
}

const char *Payload::payloadName(int type)
{
	return seClassVector()->value(type).constData();
}

AbstractPayloadFactory::AbstractPayloadFactory()
{
}

AbstractPayloadFactory::~AbstractPayloadFactory()
{
}
}
