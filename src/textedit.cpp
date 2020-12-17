/*
 * Copyright (C) Andrea Diamantini 2020 <adjam@protonmail.com>
 * Based on Code Editor Example: https://doc.qt.io/qt-5/qtwidgets-widgets-codeeditor-example.html
 *
 * CutePad project
 *
 * @license GPL-3.0 <https://www.gnu.org/licenses/gpl-3.0.txt>
 */
 
 
#include "textedit.h"

#include <QPainter>
#include <QTextBlock>


TextEdit::TextEdit(QWidget *parent)
    : QPlainTextEdit(parent)
    , _lineNumberArea(nullptr)
    , _highlight(false)
{
}


void TextEdit::enableLineNumbers(bool on)
{
	if (on) {
		_lineNumberArea = new LineNumberArea(this);
		_lineNumberArea->show();
		connect(this, &TextEdit::blockCountChanged, this, &TextEdit::updateLineNumberAreaWidth);
		connect(this, &TextEdit::updateRequest, this, &TextEdit::updateLineNumberArea);
		
		updateLineNumberAreaWidth(0);
	}
	else {
		disconnect(this, &TextEdit::blockCountChanged, this, &TextEdit::updateLineNumberAreaWidth);
		disconnect(this, &TextEdit::updateRequest, this, &TextEdit::updateLineNumberArea);
		
		setViewportMargins(0, 0, 0, 0);	
		delete _lineNumberArea;
		_lineNumberArea = nullptr;
	}
}


bool TextEdit::isLineNumbersEnabled()
{
	return _lineNumberArea ? true : false;
}


void TextEdit::enableCurrentLineHighlighting(bool on)
{
	if (_highlight == on)
		return;
		
	_highlight = on;
	if (on) {
		highlightCurrentLine();
		connect(this, &TextEdit::cursorPositionChanged, this, &TextEdit::highlightCurrentLine);
	} else {
		QList<QTextEdit::ExtraSelection> extraSelections;
		
		if (!isReadOnly()) {
    		QTextEdit::ExtraSelection selection;
		
			// FIXME: choose right background color
    		QColor lineColor = QColor(Qt::white).lighter(160);
    		selection.format.setBackground(lineColor);
    		selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    		selection.cursor = textCursor();
    		selection.cursor.clearSelection();
    		extraSelections.append(selection);
		}
		setExtraSelections(extraSelections);

		disconnect(this, &TextEdit::cursorPositionChanged, this, &TextEdit::highlightCurrentLine);
	}
}


bool TextEdit::isCurrentLineHighlightingEnabled()
{
	return _highlight;
}


void TextEdit::lineNumberAreaPaintEvent (QPaintEvent *event)
{
	QPainter painter(_lineNumberArea);
	painter.fillRect(event->rect(), Qt::lightGray);
	
	QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();
    
     while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, _lineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}


int TextEdit::lineNumberAreaWidth()
{
	int digits = 2;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }
    
    int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
    return space;
}


void TextEdit::resizeEvent(QResizeEvent *e)
{
	QPlainTextEdit::resizeEvent(e);
	
	if (_lineNumberArea) {
		QRect cr = contentsRect();
		_lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
	}
}


void TextEdit::updateLineNumberAreaWidth(int /*newBlockCount*/)
{
	setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}


void TextEdit::highlightCurrentLine()
{
	QList<QTextEdit::ExtraSelection> extraSelections;
	
	if (!isReadOnly()) {
    	QTextEdit::ExtraSelection selection;
	
		// TODO: let user change line highlight color
    	QColor lineColor = QColor(Qt::yellow).lighter(160);
	
    	selection.format.setBackground(lineColor);
    	selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    	selection.cursor = textCursor();
    	selection.cursor.clearSelection();
    	extraSelections.append(selection);
	}
	
	setExtraSelections(extraSelections);
}


void TextEdit::updateLineNumberArea(const QRect &rect, int dy)
{
	if (!_lineNumberArea)
		return;
		
    if (dy)
        _lineNumberArea->scroll(0, dy);
    else
        _lineNumberArea->update(0, rect.y(), _lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}
