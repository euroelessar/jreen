/****************************************************************************
 *  jidedit.h
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

#ifndef JIDEDIT_H
#define JIDEDIT_H

#include <QWidget>
#include "../jid.h"

class QMenu;
class QResizeEvent;

namespace Jreen
{

class JidEditPrivate;

class JREEN_EXPORT JidEdit : public QWidget
{
	Q_OBJECT
	Q_PROPERTY(JID jid READ jid WRITE setJid NOTIFY jidChanged USER true)
	Q_PROPERTY(bool frame READ hasFrame WRITE setFrame)
	Q_PROPERTY(int cursorPosition READ cursorPosition WRITE setCursorPosition)
	Q_PROPERTY(Qt::Alignment alignment READ alignment WRITE setAlignment)
	Q_PROPERTY(bool modified READ isModified WRITE setModified DESIGNABLE false)
	Q_PROPERTY(bool hasSelectedText READ hasSelectedText)
	Q_PROPERTY(QString selectedText READ selectedText)
	Q_PROPERTY(bool dragEnabled READ dragEnabled WRITE setDragEnabled)
	Q_PROPERTY(bool readOnly READ isReadOnly WRITE setReadOnly)
	Q_PROPERTY(bool undoAvailable READ isUndoAvailable)
	Q_PROPERTY(bool redoAvailable READ isRedoAvailable)

public:
	JidEdit(QWidget *parent = 0);
	JidEdit(const JID &jid, QWidget *parent = 0);
	virtual ~JidEdit();

	const JID &jid() const;

	void setFrame(bool);
	bool hasFrame() const;

	bool isReadOnly() const;
	void setReadOnly(bool);

	QSize sizeHint() const;
	QSize minimumSizeHint() const;

	int cursorPosition() const;
	void setCursorPosition(int);
	int cursorPositionAt(const QPoint &pos);

	void setAlignment(Qt::Alignment flag);
	Qt::Alignment alignment() const;

	void cursorForward(bool mark, int steps = 1);
	void cursorBackward(bool mark, int steps = 1);
	void cursorWordForward(bool mark);
	void cursorWordBackward(bool mark);
	void backspace();
	void del();
	void home(bool mark);
	void end(bool mark);

	bool isModified() const;
	void setModified(bool);

	void setSelection(int, int);
	bool hasSelectedText() const;
	QString selectedText() const;
	int selectionStart() const;

	bool isUndoAvailable() const;
	bool isRedoAvailable() const;

	void setDragEnabled(bool b);
	bool dragEnabled() const;

	void setTextMargins(int left, int top, int right, int bottom);
	void getTextMargins(int *left, int *top, int *right, int *bottom) const;

public slots:
	void setJid(const JID &);
	void clear();
	void selectAll();
	void undo();
	void redo();
#ifndef QT_NO_CLIPBOARD
	void cut();
	void copy() const;
	void paste();
#endif

public:
	void deselect();
	void insert(const QString &);
#ifndef QT_NO_CONTEXTMENU
	QMenu *createStandardContextMenu();
#endif

signals:
	void jidChanged(const JID &);
	void jidEdited(const JID &);
	void cursorPositionChanged(int, int);
	void returnPressed();
	void editingFinished();
	void selectionChanged();

protected:
	void resizeEvent(QResizeEvent *event);
	QRect cursorRect() const;

private:
	Q_DECLARE_PRIVATE(JidEdit)
	Q_DISABLE_COPY(JidEdit)
	QScopedPointer<JidEditPrivate> d_ptr;
};

}

#endif // JIDEDIT_H
