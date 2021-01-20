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

    // 0 = hide (default), 1 = show, 2 = smart
    void setLineNumbersMode(int mode);
    int lineNumbersMode();

    void enableCurrentLineHighlighting(bool on);
    bool isCurrentLineHighlightingEnabled();

    void setHighlightLineColor(const QColor& color);
    QColor highlightLineColor();
    
    // -------------------------------------------------------------------
    void lineNumberAreaPaintEvent (QPaintEvent *event);
    int lineNumberAreaWidth();

    void checkTabSpaceReplacementNeeded();
    bool isTabReplacementEnabled();

    void setTabsCount(int tabsCount);
    int tabsCount();
    
public slots:
    void enableTabReplacement(bool on);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &, int);

private:
    QWidget* _lineNumberArea;

    int _lineNumbersMode;
    
    bool _highlight;
    QColor _highlightLineColor;
    
    bool _tabReplace;
    QString _spaces;
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

    QSize sizeHint() const override {
        return QSize (_editor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent (QPaintEvent *event) override {
        _editor->lineNumberAreaPaintEvent (event);
    }

private:
    TextEdit* _editor;
};


#endif // TEXTEDIT_H
