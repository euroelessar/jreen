#include "stanzaextension.h"

J_BEGIN_NAMESPACE

static int stanza_extension_count = 0;

StanzaExtensionMeta::StanzaExtensionMeta( const char *s, const char *s2 ) : name(s), xpath(QLatin1String(s2)), type(s?stanza_extension_count++:-1)
{
	qDebug( "StanzaExtensionMeta: \"%s\", %d", name.constData(), type );
}


J_END_NAMESPACE
