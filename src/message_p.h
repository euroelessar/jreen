#ifndef MESSAGE_P_H
#define MESSAGE_P_H
#include "stanza_p.h"
#include "langmap.h"

namespace jreen
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

} //namespace jreen

#endif // MESSAGE_P_H
