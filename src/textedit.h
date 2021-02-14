/*
 * Copyright (C) Andrea Diamantini 2020-2021 <adjam@protonmail.com>
 * Based on CodeEditor Example: https://doc.qt.io/qt-5/qtwidgets-widgets-codeeditor-example.html
 *
 * CutePad project
 *
 * @license GPL-3.0 <https://www.gnu.org/licenses/gpl-3.0.txt>
 */


#ifndef TEXTEDIT_H
#define TEXTEDIT_H


#include <QPlainTextEdit>

#include <KSyntaxHighlighting/Repository>
#include <KSyntaxHighlighting/SyntaxHighlighter>

class QTextCodec;


class TextEdit : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit TextEdit(QWidget *parent = nullptr);

    void loadFilePath(const QString & path);
    void saveFilePath(const QString & path);

    QTextCodec* textCodec();
    void encode(QTextCodec* targetCodec);

    inline QString language() const { return _language; };

    // 0 = hide (default), 1 = show, 2 = smart (show with code, hide with plain text)
    void setLineNumbersMode(int mode);
    int lineNumbersMode();

    void setCurrentLineHighlightingEnabled(bool on);
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
    
public Q_SLOTS:
    void enableTabReplacement(bool on);
    void updateLineNumbersMode();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private Q_SLOTS:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &, int);

    // enable syntax highlighting
    void syntaxHighlightForFile(const QString & path);

private:
    QWidget* _lineNumberArea;

    KSyntaxHighlighting::SyntaxHighlighter* _highlighter;
    KSyntaxHighlighting::Repository* _highlightRepo;

    QString _language;

    int _lineNumbersMode;
    
    bool _highlight;
    QColor _highlightLineColor;
    
    bool _tabReplace;
    QString _spaces;

    QTextCodec* _textCodec;
};


// ------------------------------------------------------------------------------------


class LineNumberArea : public QWidget
{
    Q_OBJECT

public:
    explicit LineNumberArea (TextEdit *editor)
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
