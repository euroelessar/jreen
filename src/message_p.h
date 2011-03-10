#ifndef MESSAGE_P_H
#define MESSAGE_P_H
#include "stanza_p.h"
#include "langmap.h"

namespace Jreen
{

class MessagePrivate : public StanzaPrivate
{
public:
	MessagePrivate() : StanzaPrivate(StanzaMessage) {}
	Message::Type subtype;
	LangMap body;
	LangMap subject;
	QString thread;
};

} //namespace Jreen

#endif // MESSAGE_P_H
