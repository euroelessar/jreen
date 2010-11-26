/****************************************************************************
*  mood.h
*
*  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
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

#ifndef MOOD_H
#define MOOD_H
#include "stanzaextension.h"

namespace jreen {

class JREEN_EXPORT Mood : public StanzaExtension
{
	J_EXTENSION(jreen::Mood,"/message/mood[@xmlns='http://jabber.org/protocol/mood']")
public:
	enum Type {
			Afraid = 0,
			Amazed,
			Angry,
			Amorous,
			Annoyed,
			Anxious,
			Aroused,
			Ashamed,
			Bored,
			Brave,
			Calm,
			Cautious,
			Cold,
			Confident,
			Confused,
			Contemplative,
			Contented,
			Cranky,
			Crazy,
			Creative,
			Curious,
			Dejected,
			Depressed,
			Disappointed,
			Disgusted,
			Dismayed,
			Distracted,
			Embarrassed,
			Envious,
			Excited,
			Flirtatious,
			Frustrated,
			Grumpy,
			Guilty,
			Happy,
			Hopeful,
			Hot,
			Humbled,
			Humiliated,
			Hungry,
			Hurt,
			Impressed,
			In_awe,
			In_love,
			Indignant,
			Interested,
			Intoxicated,
			Invincible,
			Jealous,
			Lonely,
			Lucky,
			Mean,
			Moody,
			Nervous,
			Neutral,
			Offended,
			Outraged,
			Playful,
			Proud,
			Relaxed,
			Relieved,
			Remorseful,
			Restless,
			Sad,
			Sarcastic,
			Serious,
			Shocked,
			Shy,
			Sick,
			Sleepy,
			Spontaneous,
			Stressed,
			Strong,
			Surprised,
			Thankful,
			Thirsty,
			Tired,
			Undefined,
			Weak,
			Worried,
			Invalid = -1
	};
	Mood(Type type,const QString &text)
	: m_type(type),m_text(text) {}
	Type subtype() const {return m_type;}
	QString text() const {return m_text;}
private:
	Type m_type;
	QString m_text;
};

} // namespace jreen

#endif // MOOD_H
