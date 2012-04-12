/****************************************************************************
 *  jidedit.cpp
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

#include "jidedit_p.h"
#include <QResizeEvent>

namespace Jreen
{

void JidValidator::fixup(QString &input) const
{
	input = JID(input);
}

QValidator::State JidValidator::validate(QString &input, int &pos) const
{
	Q_UNUSED(pos);
	JID jid(input);
	if(jid == input)
		return QValidator::Acceptable;
	if(jid.isValid())
	{
		input = jid;
		return QValidator::Intermediate;
	}
	return QValidator::Intermediate;
}

JidEdit::JidEdit(QWidget *parent) : QWidget(parent), d_ptr(new JidEditPrivate)
{
	Q_D(JidEdit);
	d->line_edit = new LineEditHelper(QString(), this);
	d->validator = new JidValidator(this);
	d->line_edit->setValidator(d->validator);
}

JidEdit::JidEdit(const JID &jid, QWidget *parent) : QWidget(parent), d_ptr(new JidEditPrivate)
{
	Q_D(JidEdit);
	d->line_edit = new LineEditHelper(jid, this);
	d->jid = jid;
	d->validator = new JidValidator(this);
	d->line_edit->setValidator(d->validator);
}

JidEdit::~JidEdit()
{
}

void JidEdit::resizeEvent(QResizeEvent *event)
{
	Q_D(JidEdit);
	d->line_edit->resize(event->size());
}

const JID &JidEdit::jid() const
{
	Q_D(const JidEdit);
	return d->jid;
}

void JidEdit::setJid(const JID &jid)
{
	Q_D(JidEdit);
	d->jid = jid;
	d->line_edit->setText(jid);
}

void JidEdit::setFrame(bool b)
{
	Q_D(JidEdit);
	d->line_edit->setFrame(b);
}

bool JidEdit::hasFrame() const
{
	Q_D(const JidEdit);
	return d->line_edit->hasFrame();
}

bool JidEdit::isReadOnly() const
{
	Q_D(const JidEdit);
	return d->line_edit->isReadOnly();
}

void JidEdit::setReadOnly(bool b)
{
	Q_D(JidEdit);
	d->line_edit->setReadOnly(b);
}

QSize JidEdit::sizeHint() const
{
	Q_D(const JidEdit);
	return d->line_edit->sizeHint();
}

QSize JidEdit::minimumSizeHint() const
{
	Q_D(const JidEdit);
	return d->line_edit->minimumSizeHint();
}

int JidEdit::cursorPosition() const
{
	Q_D(const JidEdit);
	return d->line_edit->cursorPosition();
}

void JidEdit::setCursorPosition(int i)
{
	Q_D(JidEdit);
	return d->line_edit->setCursorPosition(i);
}

int JidEdit::cursorPositionAt(const QPoint &pos)
{
	Q_D(JidEdit);
	return d->line_edit->cursorPositionAt(pos);
}

void JidEdit::setAlignment(Qt::Alignment flag)
{
	Q_D(JidEdit);
	d->line_edit->setAlignment(flag);
}

Qt::Alignment JidEdit::alignment() const
{
	Q_D(const JidEdit);
	return d->line_edit->alignment();
}


void JidEdit::cursorForward(bool mark, int steps)
{
	Q_D(JidEdit);
	d->line_edit->cursorForward(mark, steps);
}

void JidEdit::cursorBackward(bool mark, int steps)
{
	Q_D(JidEdit);
	d->line_edit->cursorBackward(mark, steps);
}

void JidEdit::cursorWordForward(bool mark)
{
	Q_D(JidEdit);
	d->line_edit->cursorWordForward(mark);
}

void JidEdit::cursorWordBackward(bool mark)
{
	Q_D(JidEdit);
	d->line_edit->cursorWordBackward(mark);
}

void JidEdit::backspace()
{
	Q_D(JidEdit);
	d->line_edit->backspace();
}

void JidEdit::del()
{
	Q_D(JidEdit);
	d->line_edit->del();
}

void JidEdit::home(bool mark)
{
	Q_D(JidEdit);
	d->line_edit->home(mark);
}

void JidEdit::end(bool mark)
{
	Q_D(JidEdit);
	d->line_edit->end(mark);
}

bool JidEdit::isModified() const
{
	Q_D(const JidEdit);
	return d->line_edit->isModified();
}

void JidEdit::setModified(bool b)
{
	Q_D(JidEdit);
	d->line_edit->setModified(b);
}

void JidEdit::setSelection(int start, int length)
{
	Q_D(JidEdit);
	d->line_edit->setSelection(start, length);
}

bool JidEdit::hasSelectedText() const
{
	Q_D(const JidEdit);
	return d->line_edit->hasSelectedText();
}

QString JidEdit::selectedText() const
{
	Q_D(const JidEdit);
	return d->line_edit->selectedText();
}

int JidEdit::selectionStart() const
{
	Q_D(const JidEdit);
	return d->line_edit->selectionStart();
}

bool JidEdit::isUndoAvailable() const
{
	Q_D(const JidEdit);
	return d->line_edit->isUndoAvailable();
}

bool JidEdit::isRedoAvailable() const
{
	Q_D(const JidEdit);
	return d->line_edit->isRedoAvailable();
}

void JidEdit::setDragEnabled(bool b)
{
	Q_D(JidEdit);
	d->line_edit->setDragEnabled(b);
}

bool JidEdit::dragEnabled() const
{
	Q_D(const JidEdit);
	return d->line_edit->dragEnabled();
}

void JidEdit::setTextMargins(int left, int top, int right, int bottom)
{
	Q_D(JidEdit);
	return d->line_edit->setTextMargins(left, top, right, bottom);
}

void JidEdit::getTextMargins(int *left, int *top, int *right, int *bottom) const
{
	Q_D(const JidEdit);
	return d->line_edit->getTextMargins(left, top, right, bottom);
}

void JidEdit::clear()
{
	Q_D(JidEdit);
	d->line_edit->clear();
}

void JidEdit::selectAll()
{
	Q_D(JidEdit);
	d->line_edit->selectAll();
}

void JidEdit::undo()
{
	Q_D(JidEdit);
	d->line_edit->undo();
}

void JidEdit::redo()
{
	Q_D(JidEdit);
	d->line_edit->redo();
}

#ifndef QT_NO_CLIPBOARD
void JidEdit::cut()
{
	Q_D(JidEdit);
	d->line_edit->cut();
}

void JidEdit::copy() const
{
	Q_D(const JidEdit);
	d->line_edit->copy();
}

void JidEdit::paste()
{
	Q_D(JidEdit);
	d->line_edit->paste();
}
#endif

void JidEdit::deselect()
{
	Q_D(JidEdit);
	d->line_edit->deselect();
}

void JidEdit::insert(const QString &text)
{
	Q_D(JidEdit);
	d->line_edit->insert(text);
}

#ifndef QT_NO_CONTEXTMENU
QMenu *JidEdit::createStandardContextMenu()
{
	Q_D(JidEdit);
	return d->line_edit->createStandardContextMenu();
}
#endif

QRect JidEdit::cursorRect() const
{
	Q_D(const JidEdit);
	return d->line_edit->cursorRect();
}


}
