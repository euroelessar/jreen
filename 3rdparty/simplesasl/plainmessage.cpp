/*
 * Copyright © 2008  Remko Troncon <remko@el-tramo.be>
 * See COPYING for license details.
 */

#include "plainmessage.h"

namespace XMPP {

PLAINMessage::PLAINMessage(const QString& authzid, const QString& authcid, const QByteArray& password)
{
	value_ = authzid.toUtf8() + '\0' + authcid.toUtf8() + '\0' + password;
}

}
