/*
 * Copyright (C) Andrea Diamantini 2020 <adjam@protonmail.com>
 * Based on Code Editor Example: https://doc.qt.io/qt-5/qtwidgets-widgets-codeeditor-example.html
 *
 * CutePad project
 *
 * @license GPL-3.0 <https://www.gnu.org/licenses/gpl-3.0.txt>
 */


#include "textedit.h"

#include <KSyntaxHighlighting/Theme>
#include <KSyntaxHighlighting/Definition>

#include <QMessageBox>
#include <QPainter>
#include <QTextBlock>

#include <QDebug>


TextEdit::TextEdit(QWidget *parent)
    : QPlainTextEdit(parent)
    , _highlighter(new KSyntaxHighlighting::SyntaxHighlighter(this->document()))
    , _highlightRepo(new KSyntaxHighlighting::Repository)
    , _language("none")
    , _lineNumberArea(nullptr)
    , _lineNumbersMode(0)
    , _highlight(false)
    , _tabReplace(false)
{
}


void TextEdit::syntaxHighlightForFile(const QString & path)
{
    _highlighter->setTheme((palette().color(QPalette::Base).lightness() < 128)
                     ? _highlightRepo->defaultTheme(KSyntaxHighlighting::Repository::DarkTheme)
                     : _highlightRepo->defaultTheme(KSyntaxHighlighting::Repository::LightTheme));

    const auto def = _highlightRepo->definitionForFileName(path);
    if (!def.isValid()) {
        qDebug() << "no valid definitions found :(";
        _language = "none";
        return;
    }

    _highlighter->setDefinition(def);

    // consider moving to translatedName()
    _language = def.name();
}


void TextEdit::checkTabSpaceReplacementNeeded()
{
    if (!_tabReplace)
        return;

    QString content = toPlainText();

    if (!content.contains( QChar(QChar::Tabulation) )) {
        return;
    }

    int risp = QMessageBox::question(this,
                                     "Replace Tabs with Spaces",
                                     "Your document contains tabs. Would you like them to be replaced with spaces?",
                                     QMessageBox::Yes | QMessageBox::No);

    if (risp == QMessageBox::No) {
        return;
    }

    QTextCursor actual = textCursor();
    content.replace( QChar(QChar::Tabulation) , _spaces);
    setPlainText(content);
    setTextCursor(actual);
    document()->setModified(true);
}


bool TextEdit::isTabReplacementEnabled()
{
    return _tabReplace;
}


void TextEdit::enableTabReplacement(bool on)
{
    _tabReplace = on;
    checkTabSpaceReplacementNeeded();
}


void TextEdit::setLineNumbersMode(int mode)
{
    _lineNumbersMode = mode;
    updateLineNumbersMode();
}


void TextEdit::updateLineNumbersMode()
{
    bool enable;

    switch (_lineNumbersMode)
    {
    case 0:
        enable = false;
        break;
    case 1:
        enable = true;
        break;
    case 2:
        enable = (_language != "none");
    default:
        // this should NEVER happen...
        break;
    }

    bool enabled = _lineNumberArea;
    if (enable == enabled) {
        qDebug() << "we'd like to enable/disable line numbers but they just are...";
        return;
    }

    // show
    if (enable) {
        _lineNumberArea = new LineNumberArea(this);
        _lineNumberArea->show();
        connect(this, &TextEdit::blockCountChanged, this, &TextEdit::updateLineNumberAreaWidth);
        connect(this, &TextEdit::updateRequest, this, &TextEdit::updateLineNumberArea);

        updateLineNumberAreaWidth(0);
        return;
    }
    
    // hide
    disconnect(this, &TextEdit::blockCountChanged, this, &TextEdit::updateLineNumberAreaWidth);
    disconnect(this, &TextEdit::updateRequest, this, &TextEdit::updateLineNumberArea);

    setViewportMargins(0, 0, 0, 0);
    delete _lineNumberArea;
    _lineNumberArea = nullptr;
}


int TextEdit::lineNumbersMode()
{
    return _lineNumbersMode;
}


void TextEdit::enableCurrentLineHighlighting(bool on)
{
    if (_highlight == on)
        return;

    _highlight = on;    
    if (on) {
        highlightCurrentLine();
        connect(this, &TextEdit::cursorPositionChanged, this, &TextEdit::highlightCurrentLine);
        return;
    }
    // else... hide!!!
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


void TextEdit::keyPressEvent(QKeyEvent *event)
{
    // TAB: (eventually) replace with spaces
    // TAB: if there is a selection, move it
    if (event->key() == Qt::Key_Tab) {

        QString indentation = _tabReplace ? _spaces : QString(QChar(QChar::Tabulation));

        if (!textCursor().hasSelection()) {
            qDebug() << "TAB pressed, NO selection";
            QTextCursor cur = textCursor();
            cur.beginEditBlock();
            cur.insertText (indentation);
            cur.endEditBlock();
            setTextCursor(cur);
            event->accept();
            return;
        } else {

            QTextCursor cur = textCursor();
            QString selection = cur.selectedText();
            cur.removeSelectedText();            
            QStringList rows = selection.split(QChar (QChar::ParagraphSeparator));
            int start = cur.position();
            cur.beginEditBlock();
            for (int i = 0; i < rows.size(); i++) {
                QString row = rows.at(i);
                cur.insertText(indentation);
                cur.insertText (row);
                if (i != rows.size() - 1) {
                    cur.insertText( QChar (QChar::ParagraphSeparator) );
                }
            }
            cur.endEditBlock();
            int end = cur.position();
            cur.setPosition(start);
            cur.setPosition(end, QTextCursor::KeepAnchor);
            setTextCursor(cur);
            event->accept();
            return;
        }
    }
    
    if (event->key() == Qt::Key_Backtab) {
        if (!textCursor().hasSelection()) {
            event->ignore();
            return;
        }

        QString indentation = _tabReplace ? _spaces : QString(QChar (QChar::Tabulation));

        QTextCursor cur = textCursor();
        QString selection = cur.selectedText();
        cur.removeSelectedText();            
        QStringList rows = selection.split(QChar (QChar::ParagraphSeparator));
        int start = cur.position();
        cur.beginEditBlock();
        for (int i = 0; i < rows.size(); i++) {
            QString row = rows.at(i);
            if (row.startsWith(indentation)) {
                row.remove(0, indentation.size());
            }
            cur.insertText (row);
            if (i != rows.size() - 1) {
                cur.insertText( QChar (QChar::ParagraphSeparator) );
            }
        }
        cur.endEditBlock();
        int end = cur.position();
        cur.setPosition(start);
        cur.setPosition(end, QTextCursor::KeepAnchor);
        setTextCursor(cur);
        event->accept();
        return;
    }

    // ENTER / RETURN: preserve indentation
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        QTextCursor actual = textCursor();
        QTextCursor cur = actual;
        cur.movePosition(QTextCursor::StartOfLine);
        QString indentation;
        while (!cur.atBlockEnd()) {
            cur.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
            QString sel = cur.selectedText();
            if (sel != QString(QChar(QChar::Space)) && sel != QString(QChar(QChar::Tabulation)) ) {
                break;
            }
            indentation.append(sel);
            cur.clearSelection();    // remember to reset anchor
        }

        // go to next line and add indentation
        actual.beginEditBlock();
        actual.insertText (QChar (QChar::ParagraphSeparator));
        actual.insertText (indentation);
        actual.endEditBlock();

        setTextCursor(actual);
        event->accept();
        return;
    }

    return QPlainTextEdit::keyPressEvent(event);
}


void TextEdit::resizeEvent(QResizeEvent *event)
{
    QPlainTextEdit::resizeEvent(event);

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
        selection.format.setBackground(_highlightLineColor);
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


void TextEdit::setHighlightLineColor(const QColor& color)
{
    _highlightLineColor = color;
    
    highlightCurrentLine();
}


QColor TextEdit::highlightLineColor()
{
    return _highlightLineColor;
}


void TextEdit::setTabsCount(int tabsCount)
{
    _spaces.clear();
    for (int i = 0; i < tabsCount; i++) {
        _spaces.append( QChar(QChar::Space) );
    }
}


int TextEdit::tabsCount()
{
    return _spaces.count();
}
