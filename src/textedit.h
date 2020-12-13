/*
 * Copyright (C) Andrea Diamantini 2020 <adjam@protonmail.com>
 * Based on CodeEditor Example: https://doc.qt.io/qt-5/qtwidgets-widgets-codeeditor-example.html
 *
 * CutePad project
 *
 * @license GPL-3.0 <https://www.gnu.org/licenses/gpl-3.0.txt>
 */
 

#ifndef TEXTEDIT_H
#define TEXTEDIT_H


#include <QPlainTextEdit>


class TextEdit : public QPlainTextEdit
{
    Q_OBJECT

public:
    TextEdit(QWidget *parent = nullptr);
    
    void lineNumberAreaPaintEvent (QPaintEvent *event);
    int lineNumberAreaWidth();

protected:
    void resizeEvent(QResizeEvent *event);

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &, int);

private:
    QWidget* _lineNumberArea;
};


// ------------------------------------------------------------------------------------


class LineNumberArea : public QWidget
{
    Q_OBJECT

public:
    LineNumberArea (TextEdit *editor) 
    	: QWidget (editor) {
        _editor = editor;
    }

    QSize sizeHint() const {
        return QSize (_editor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent (QPaintEvent *event) {
        _editor->lineNumberAreaPaintEvent (event);
    }

private:
    TextEdit *_editor;
};


#endif // TEXTEDIT_H
