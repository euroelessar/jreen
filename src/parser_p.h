/*****************************************************************************
 *  parser_p.h
 *
 *  Copyright (c) 2003 by Justin Karneges
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#ifndef PARSER_P_H
#define PARSER_P_H

#include <QXmlDefaultHandler>
#include <QDomDocument>
#include <QTextDecoder>
#include "jreen.h"
#include "parser.h"

/*
 * WARNING!
 * This file is not a part of JReen API, it may be chagned or even removed
 * without any notification.
 */

J_BEGIN_NAMESPACE

class Parser::EventPrivate
{
public:
	EventPrivate()
	{
		ref = 1;
	}
	QAtomicInt ref;
	Parser::Event::Type type;
	QString ns, ln, qn;
	QXmlAttributes a;
	QDomElement e;
	QString str;
	QStringList nsnames, nsvalues;
};

//----------------------------------------------------------------------------
// StreamInput
//----------------------------------------------------------------------------
class StreamInput : public QXmlInputSource
{
public:
	StreamInput()
	{
		dec = 0;
		reset();
	}

	~StreamInput()
	{
		delete dec;
	}

	void reset()
	{
		delete dec;
		dec = 0;
		in.resize(0);
		out.clear();
		at = 0;
		paused = false;
		mightChangeEncoding = true;
		checkBad = true;
		last = QChar();
		v_encoding.clear();
		resetLastData();
	}

	void resetLastData()
	{
		last_string.clear();
	}

	QString lastString() const
	{
		return last_string;
	}

	void appendData(const QByteArray &a)
	{
		int oldsize = in.size();
		in.resize(oldsize + a.size());
		memcpy(in.data() + oldsize, a.data(), a.size());
		processBuf();
	}

	QChar lastRead()
	{
		return last;
	}

	QChar next()
	{
		if( paused )
			return EndOfData;
		else
			return readNext();
	}

	// NOTE: setting 'peek' to true allows the same char to be read again,
	//       however this still advances the internal byte processing.
	QChar readNext(bool peek=false)
	{
		QChar c;
		if( mightChangeEncoding )
			c = EndOfData;
		else {
			if( out.isEmpty() ) {
				QString s;
				if( !tryExtractPart(&s) )
					c = EndOfData;
				else {
					out = s;
					c = out[0];
				}
			}
			else
				c = out[0];
			if( !peek )
				out.remove(0, 1);
		}
		if( c == EndOfData ) {
#ifdef XMPP_PARSER_DEBUG
			printf("next() = EOD\n");
#endif
		}
		else {
#ifdef XMPP_PARSER_DEBUG
			printf("next() = [%c]\n", c.toLatin1());
#endif
			last = c;
		}

		return c;
	}

	QByteArray unprocessed() const
	{
		QByteArray a;
		a.resize(in.size() - at);
		memcpy(a.data(), in.data() + at, a.size());
		return a;
	}

	void pause(bool b)
	{
		paused = b;
	}

	bool isPaused()
	{
		return paused;
	}

	QString encoding() const
	{
		return v_encoding;
	}

private:
	QTextDecoder *dec;
	QByteArray in;
	QString out;
	int at;
	bool paused;
	bool mightChangeEncoding;
	QChar last;
	QString v_encoding;
	QString last_string;
	bool checkBad;

	void processBuf()
	{
#ifdef XMPP_PARSER_DEBUG
		printf("processing.  size=%d, at=%d\n", in.size(), at);
#endif
		if( !dec ) {
			QTextCodec *codec = 0;
			uchar *p = (uchar *)in.data() + at;
			int size = in.size() - at;

			// do we have enough information to determine the encoding?
			if( size == 0 )
				return;
			bool utf16 = false;
			if( p[0] == 0xfe || p[0] == 0xff ) {
				// probably going to be a UTF-16 byte order mark
				if( size < 2 )
					return;
				if( (p[0] == 0xfe && p[1] == 0xff) || (p[0] == 0xff && p[1] == 0xfe) ) {
					// ok it is UTF-16
					utf16 = true;
				}
			}
			if( utf16 )
				codec = QTextCodec::codecForMib(1000); // UTF-16
			else
				codec = QTextCodec::codecForMib(106); // UTF-8

			v_encoding = codec->name();
			dec = codec->makeDecoder();

			// for utf16, put in the byte order mark
			if( utf16 ) {
				out += dec->toUnicode((const char *)p, 2);
				at += 2;
			}
		}

		if( mightChangeEncoding ) {
			while(1) {
				int n = out.indexOf('<');
				if( n != -1 ) {
					// we need a closing bracket
					int n2 = out.indexOf('>', n);
					if( n2 != -1 ) {
						++n2;
						QString h = out.mid(n, n2-n);
						QString enc = processXmlHeader(h);
						QTextCodec *codec = 0;
						if( !enc.isEmpty() )
							codec = QTextCodec::codecForName(enc.toLatin1());

						// changing codecs
						if( codec) {
							v_encoding = codec->name();
							delete dec;
							dec = codec->makeDecoder();
						}
						mightChangeEncoding = false;
						out.truncate(0);
						at = 0;
						resetLastData();
						break;
					}
				}
				QString s;
				if( !tryExtractPart(&s) )
					break;
				if( checkBad && checkForBadChars(s) ) {
					// go to the parser
					mightChangeEncoding = false;
					out.truncate(0);
					at = 0;
					resetLastData();
					break;
				}
				out += s;
			}
		}
	}

	QString processXmlHeader(const QString &h)
	{
		if( h.left(5) != "<?xml" )
			return "";

		int endPos = h.indexOf(">");
		int startPos = h.indexOf("encoding");
		if( startPos < endPos && startPos != -1 ) {
			QString encoding;
			do {
				startPos++;
				if( startPos > endPos ) {
					return "";
				}
			} while(h[startPos] != '"' && h[startPos] != '\'');
			startPos++;
			while(h[startPos] != '"' && h[startPos] != '\'') {
				encoding += h[startPos];
				startPos++;
				if( startPos > endPos ) {
					return "";
				}
			}
			return encoding;
		}
		else
			return "";
	}

	bool tryExtractPart(QString *s)
	{
		int size = in.size() - at;
		if( size == 0 )
			return false;
		uchar *p = (uchar *)in.data() + at;
		QString nextChars;
		while(1) {
			nextChars = dec->toUnicode((const char *)p, 1);
			++p;
			++at;
			if( !nextChars.isEmpty() )
				break;
			if( at == (int)in.size() )
				return false;
		}
		last_string += nextChars;
		*s = nextChars;

		// free processed data?
		if( at >= 1024 ) {
			char *p = in.data();
			int size = in.size() - at;
			memmove(p, p + at, size);
			in.resize(size);
			at = 0;
		}

		return true;
	}

	bool checkForBadChars(const QString &s)
	{
		int len = s.indexOf('<');
		if( len == -1 )
			len = s.length();
		else
			checkBad = false;
		for(int n = 0; n < len; ++n) {
			if( !s.at(n).isSpace() )
				return true;
		}
		return false;
	}
};

//----------------------------------------------------------------------------
// ParserHandler
//----------------------------------------------------------------------------

class ParserHandler : public QObject, public QXmlDefaultHandler
{
	Q_OBJECT
public:
	ParserHandler(StreamInput *in, QDomDocument *doc)
	{
		this->in = in;
		this->doc = doc;
		needMore = false;
	}

	~ParserHandler()
	{
	}

	bool startDocument()
	{
		depth = 0;
		return true;
	}

	bool endDocument()
	{
		return true;
	}

	bool startPrefixMapping(const QString &prefix, const QString &uri)
	{
		if( depth == 0 ) {
			nsnames += prefix;
			nsvalues += uri;
		}
		return true;
	}

	bool startElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &atts)
	{
		if( depth == 0)  {
			Parser::Event *e = new Parser::Event;
			QXmlAttributes a;
			for(int n = 0; n < atts.length(); ++n) {
				QString uri = atts.uri(n);
				QString ln = atts.localName(n);
				if( a.index(uri, ln) == -1 )
					a.append(atts.qName(n), uri, ln, atts.value(n));
			}
			e->setDocumentOpen(namespaceURI, localName, qName, a, nsnames, nsvalues);
			nsnames.clear();
			nsvalues.clear();
			e->setActualString(in->lastString());

			in->resetLastData();
			eventList.append(e);
			in->pause(true);
		}
		else {
			QDomElement e = doc->createElementNS(namespaceURI, qName);
			for(int n = 0; n < atts.length(); ++n) {
				QString uri = atts.uri(n);
				QString ln = atts.localName(n);
				bool have;
				if( !uri.isEmpty() ) {
					have = e.hasAttributeNS(uri, ln);
				}
				else
					have = e.hasAttribute(ln);
				if( !have )
					e.setAttributeNS(uri, atts.qName(n), atts.value(n));
			}

			if( depth == 1 ) {
				elem = e;
				current = e;
			}
			else {
				current.appendChild(e);
				current = e;
			}
		}
		++depth;
		return true;
	}

	bool endElement(const QString &namespaceURI, const QString &localName, const QString &qName)
	{
		--depth;
		if( depth == 0 ) {
			Parser::Event *e = new Parser::Event;
			e->setDocumentClose(namespaceURI, localName, qName);
			e->setActualString(in->lastString());
			in->resetLastData();
			eventList.append(e);
			in->pause(true);
		}
		else {
			// done with a depth 1 element?
			if( depth == 1 ) {
				Parser::Event *e = new Parser::Event;
				e->setElement(elem);
				e->setActualString(in->lastString());
				in->resetLastData();
				eventList.append(e);
				in->pause(true);

				elem = QDomElement();
				current = QDomElement();
			}
			else
				current = current.parentNode().toElement();
		}

		if( in->lastRead() == '/' )
			checkNeedMore();

		return true;
	}

	bool characters(const QString &str)
	{
		if( depth >= 1 ) {
			QString content = str;
			if( content.isEmpty() )
				return true;

			if( !current.isNull() ) {
				QDomText text = doc->createTextNode(content);
				current.appendChild(text);
			}
		}
		return true;
	}

	/*bool processingInstruction(const QString &target, const QString &data)
	{
		printf("Processing: [%s], [%s]\n", target.latin1(), data.latin1());
		in->resetLastData();
		return true;
	}*/

	void checkNeedMore()
	{
		// Here we will work around QXmlSimpleReader strangeness and self-closing tags.
		// The problem is that endElement() is called when the '/' is read, not when
		// the final '>' is read.  This is a potential problem when obtaining unprocessed
		// bytes from StreamInput after this event, as the '>' character will end up
		// in the unprocessed chunk.  To work around this, we need to advance StreamInput's
		// internal byte processing, but not the xml character data.  This way, the '>'
		// will get processed and will no longer be in the unprocessed return, but
		// QXmlSimpleReader can still read it.  To do this, we call StreamInput::readNext
		// with 'peek' mode.
		QChar c = in->readNext(true); // peek
		if( c == QXmlInputSource::EndOfData ) {
			needMore = true;
		}
		else {
			// We'll assume the next char is a '>'.  If it isn't, then
			// QXmlSimpleReader will deal with that problem on the next
			// parse.  We don't need to take any action here.
			needMore = false;
		}
	}

	Parser::Event *takeEvent()
	{
		if( needMore)
			return 0;
		if( eventList.isEmpty() )
			return 0;

		Parser::Event *e = eventList.takeFirst();
		in->pause(false);
		return e;
	}

	StreamInput *in;
	QDomDocument *doc;
	int depth;
	QStringList nsnames, nsvalues;
	QDomElement elem, current;
	QList<Parser::Event*> eventList;
	bool needMore;

signals:
	void documentOpened(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &atts, const QStringList &nsnames, const QStringList &nsvalues);
	void elementParsed(const QDomElement &element);
	void documentClosed(const QString &namespaceURI, const QString &localName, const QString &qName);
	void error();
};

class ParserPrivate
{
public:
	ParserPrivate(QObject *parent) : m_parent(parent)
	{
		doc = 0;
		in = 0;
		handler = 0;
		reader = 0;
		reset();
	}

	~ParserPrivate()
	{
		reset(false);
	}

	void reset(bool create=true)
	{
		delete reader;
		delete handler;
		delete in;
		delete doc;

		if( create ) {
			doc = new QDomDocument;
			in = new StreamInput;
			handler = new ParserHandler(in, doc);
			reader = new QXmlSimpleReader;
			reader->setContentHandler(handler);

			// initialize the reader
			in->pause(true);
			reader->parse(in, true);
			in->pause(false);
		}
	}

	QDomDocument *doc;
	StreamInput *in;
	ParserHandler *handler;
	QXmlSimpleReader *reader;
private:
	QObject *m_parent;
};

J_END_NAMESPACE

#endif // PARSER_P_H
