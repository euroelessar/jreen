/****************************************************************************
 *  jidedit_p.h
 *
 *  Copyright (c) 2009 by Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef JIDEDIT_P_H
#define JIDEDIT_P_H

#include "jidedit.h"
#include <QLineEdit>
#include <QValidator>

namespace Jreen
{

class LineEditHelper : public QLineEdit
{
	Q_OBJECT
	friend class JidEdit;
	friend class JidEditPrivate;
public:
	inline LineEditHelper(const QString &contents, QWidget *parent) : QLineEdit(contents, parent) {}
};

class JidValidator : public QValidator
{
	Q_OBJECT
public:
	inline JidValidator(QObject *parent) : QValidator(parent) {}
	virtual void fixup(QString &input) const;
	virtual State validate(QString &input, int &pos) const;
};

class JidEditPrivate : public QObject
{
	Q_OBJECT
public:
	void connect(JidEdit *edit, LineEditHelper *helper)
	{
		this->edit = edit;
		QObject::connect(helper, SIGNAL(cursorPositionChanged(int,int)), edit, SIGNAL(cursorPositionChanged(int,int)));
		QObject::connect(helper, SIGNAL(returnPressed()), edit, SIGNAL(returnPressed()));
		QObject::connect(helper, SIGNAL(editingFinished()), edit, SIGNAL(editingFinished()));
		QObject::connect(helper, SIGNAL(selectionChanged()), edit, SIGNAL(selectionChanged()));
		QObject::connect(helper, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));
		QObject::connect(helper, SIGNAL(textEdited(QString)), this, SLOT(textEdited(QString)));
	}
	JID jid;
	LineEditHelper *line_edit;
	JidValidator *validator;
	JidEdit *edit;
private slots:
	inline void textChanged(const QString &text) { jid = text; emit edit->jidChanged(jid); }
	inline void textEdited(const QString &text) { jid = text; emit edit->jidEdited(jid); }
};

}

#endif // JIDEDIT_P_H
