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

namespace Jreen
{
class MoodPrivate;
class JREEN_EXPORT Mood : public StanzaExtension
{
	Q_DECLARE_PRIVATE(Mood);
	J_EXTENSION(Jreen::Mood,"/message/mood[@xmlns='http://jabber.org/protocol/mood']")
	public:
		enum Type {
		Afraid = 0,
				Amazed,
				Amorous,
				Angry,
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
				Grateful,
				Grieving,
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
				Lost,
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
				Satisfied,
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
				Invalid = -1,
				Empty = -2
	};

	Mood(Type type = Invalid, const QString &text = QString());
	Mood(const QString &type, const QString &text);
	~Mood();

	Type type() const;
	QString typeName() const;
	void setType(const QString &type);
	void setType(Type type);

	QString text() const;
	void setText(const QString &text);
private:
	QScopedPointer<MoodPrivate> d_ptr;
};
} // namespace Jreen

#endif // MOOD_H
