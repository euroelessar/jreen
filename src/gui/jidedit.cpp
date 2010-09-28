/****************************************************************************
 *  jidedit.cpp
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

J_BEGIN_NAMESPACE

void JidValidator::fixup( QString &input ) const
{
	input = JID( input );
}

QValidator::State JidValidator::validate( QString &input, int &pos ) const
{
	Q_UNUSED(pos);
	JID jid( input );
	if( jid == input )
		return QValidator::Acceptable;
	if( jid.isValid() )
	{
		input = jid;
		return QValidator::Intermediate;
	}
	return QValidator::Intermediate;
}

JidEdit::JidEdit( QWidget *parent ) : QWidget(parent), j_ptr(new JidEditPrivate)
{
	J_D(JidEdit);
	j->line_edit = new LineEditHelper( QString(), this );
	j->validator = new JidValidator( this );
	j->line_edit->setValidator( j->validator );
}

JidEdit::JidEdit( const JID &jid, QWidget *parent ) : QWidget(parent), j_ptr(new JidEditPrivate)
{
	J_D(JidEdit);
	j->line_edit = new LineEditHelper( jid, this );
	j->jid = jid;
	j->validator = new JidValidator( this );
	j->line_edit->setValidator( j->validator );
}

JidEdit::~JidEdit()
{
	delete j_ptr;
}

void JidEdit::resizeEvent( QResizeEvent *event )
{
	J_D(JidEdit);
	j->line_edit->resize( event->size() );
}

const JID &JidEdit::jid() const
{
	J_D(const JidEdit);
	return j->jid;
}

void JidEdit::setJid( const JID &jid )
{
	J_D(JidEdit);
	j->jid = jid;
	j->line_edit->setText( jid );
}

void JidEdit::setFrame( bool b )
{
	J_D(JidEdit);
	j->line_edit->setFrame( b );
}

bool JidEdit::hasFrame() const
{
	J_D(const JidEdit);
	return j->line_edit->hasFrame();
}

bool JidEdit::isReadOnly() const
{
	J_D(const JidEdit);
	return j->line_edit->isReadOnly();
}

void JidEdit::setReadOnly( bool b )
{
	J_D(JidEdit);
	j->line_edit->setReadOnly( b );
}

QSize JidEdit::sizeHint() const
{
	J_D(const JidEdit);
	return j->line_edit->sizeHint();
}

QSize JidEdit::minimumSizeHint() const
{
	J_D(const JidEdit);
	return j->line_edit->minimumSizeHint();
}

int JidEdit::cursorPosition() const
{
	J_D(const JidEdit);
	return j->line_edit->cursorPosition();
}

void JidEdit::setCursorPosition( int i )
{
	J_D(JidEdit);
	return j->line_edit->setCursorPosition( i );
}

int JidEdit::cursorPositionAt( const QPoint &pos )
{
	J_D(JidEdit);
	return j->line_edit->cursorPositionAt( pos );
}

void JidEdit::setAlignment( Qt::Alignment flag )
{
	J_D(JidEdit);
	j->line_edit->setAlignment( flag );
}

Qt::Alignment JidEdit::alignment() const
{
	J_D(const JidEdit);
	return j->line_edit->alignment();
}


void JidEdit::cursorForward( bool mark, int steps )
{
	J_D(JidEdit);
	j->line_edit->cursorForward( mark, steps );
}

void JidEdit::cursorBackward( bool mark, int steps )
{
	J_D(JidEdit);
	j->line_edit->cursorBackward( mark, steps );
}

void JidEdit::cursorWordForward( bool mark )
{
	J_D(JidEdit);
	j->line_edit->cursorWordForward( mark );
}

void JidEdit::cursorWordBackward( bool mark )
{
	J_D(JidEdit);
	j->line_edit->cursorWordBackward( mark );
}

void JidEdit::backspace()
{
	J_D(JidEdit);
	j->line_edit->backspace();
}

void JidEdit::del()
{
	J_D(JidEdit);
	j->line_edit->del();
}

void JidEdit::home( bool mark )
{
	J_D(JidEdit);
	j->line_edit->home( mark );
}

void JidEdit::end( bool mark )
{
	J_D(JidEdit);
	j->line_edit->end( mark );
}

bool JidEdit::isModified() const
{
	J_D(const JidEdit);
	return j->line_edit->isModified();
}

void JidEdit::setModified( bool b )
{
	J_D(JidEdit);
	j->line_edit->setModified( b );
}

void JidEdit::setSelection( int start, int length )
{
	J_D(JidEdit);
	j->line_edit->setSelection( start, length );
}

bool JidEdit::hasSelectedText() const
{
	J_D(const JidEdit);
	return j->line_edit->hasSelectedText();
}

QString JidEdit::selectedText() const
{
	J_D(const JidEdit);
	return j->line_edit->selectedText();
}

int JidEdit::selectionStart() const
{
	J_D(const JidEdit);
	return j->line_edit->selectionStart();
}

bool JidEdit::isUndoAvailable() const
{
	J_D(const JidEdit);
	return j->line_edit->isUndoAvailable();
}

bool JidEdit::isRedoAvailable() const
{
	J_D(const JidEdit);
	return j->line_edit->isRedoAvailable();
}

void JidEdit::setDragEnabled( bool b )
{
	J_D(JidEdit);
	j->line_edit->setDragEnabled( b );
}

bool JidEdit::dragEnabled() const
{
	J_D(const JidEdit);
	return j->line_edit->dragEnabled();
}

void JidEdit::setTextMargins( int left, int top, int right, int bottom )
{
	J_D(JidEdit);
	return j->line_edit->setTextMargins( left, top, right, bottom );
}

void JidEdit::getTextMargins( int *left, int *top, int *right, int *bottom ) const
{
	J_D(const JidEdit);
	return j->line_edit->getTextMargins( left, top, right, bottom );
}

void JidEdit::clear()
{
	J_D(JidEdit);
	j->line_edit->clear();
}

void JidEdit::selectAll()
{
	J_D(JidEdit);
	j->line_edit->selectAll();
}

void JidEdit::undo()
{
	J_D(JidEdit);
	j->line_edit->undo();
}

void JidEdit::redo()
{
	J_D(JidEdit);
	j->line_edit->redo();
}

#ifndef QT_NO_CLIPBOARD
void JidEdit::cut()
{
	J_D(JidEdit);
	j->line_edit->cut();
}

void JidEdit::copy() const
{
	J_D(const JidEdit);
	j->line_edit->copy();
}

void JidEdit::paste()
{
	J_D(JidEdit);
	j->line_edit->paste();
}
#endif

void JidEdit::deselect()
{
	J_D(JidEdit);
	j->line_edit->deselect();
}

void JidEdit::insert( const QString &text )
{
	J_D(JidEdit);
	j->line_edit->insert( text );
}

#ifndef QT_NO_CONTEXTMENU
QMenu *JidEdit::createStandardContextMenu()
{
	J_D(JidEdit);
	return j->line_edit->createStandardContextMenu();
}
#endif

QRect JidEdit::cursorRect() const
{
	J_D(const JidEdit);
	return j->line_edit->cursorRect();
}


J_END_NAMESPACE
