/*
 * Copyright (C) Andrea Diamantini 2020 <adjam@protonmail.com>
 *
 * CutePad project
 *
 * @license GPL-3.0+ <https://spdx.org/licenses/GPL-3.0+.html>
 */


#include "mainview.h"

#include <QVBoxLayout>
#include <QTextDocument>
#include <QTextCursor>

#include <KSyntaxHighlighting/Definition>
#include <KSyntaxHighlighting/Theme>

#include <QDebug>


MainView::MainView (QWidget *parent)
    : QWidget (parent)
    , _textEdit(new TextEdit(this))
    , _searchBar(new SearchBar(this))
    , _replaceBar(new ReplaceBar(this))
    , _highlighter(new KSyntaxHighlighting::SyntaxHighlighter(_textEdit->document()))
    , _highlightRepo(new KSyntaxHighlighting::Repository)
{
    // The UI
    auto layout = new QVBoxLayout;
    layout->setContentsMargins (0, 0, 0, 0);
    layout->addWidget (_textEdit);
    layout->addWidget (_searchBar);
    layout->addWidget (_replaceBar);
    setLayout (layout);

    // let's start with the hidden bar(s)
    _searchBar->setVisible(false);
    _replaceBar->setVisible(false);

    connect(_searchBar, &SearchBar::find, this, &MainView::find);
    connect(this, &MainView::searchMessage, _searchBar, &SearchBar::searchMessage);

    connect(_replaceBar, &ReplaceBar::replace, this, &MainView::replace);
}


void MainView::syntaxHighlightForFile(const QString & path)
{
    _highlighter->setTheme((_textEdit->palette().color(QPalette::Base).lightness() < 128)
                     ? _highlightRepo->defaultTheme(KSyntaxHighlighting::Repository::DarkTheme)
                     : _highlightRepo->defaultTheme(KSyntaxHighlighting::Repository::LightTheme));

    const auto def = _highlightRepo->definitionForFileName(path);
    _highlighter->setDefinition(def);

    qDebug() << "path:" << path;
    qDebug() << "def name: " << def.name();
}


void MainView::showSearchBar()
{
    if (_replaceBar->isVisible()) {
        _replaceBar->hide();
        return;
    }

    if (_searchBar->isVisible()) {
        _searchBar->hide();
        return;
    }

    _searchBar->show();

    // if text is selected, copy to lineEdit
    QString sel = textEdit()->textCursor().selectedText();
    if (!sel.isEmpty()) {
        _searchBar->_findLineEdit->setText(sel);
    }

    _searchBar->setFocus();
}


void MainView::hideSearchBar()
{
    _searchBar->hide();
}


bool MainView::isSearchBarActive()
{
    return _searchBar->isVisible();
}


void MainView::showReplaceBar()
{
    if (_replaceBar->isVisible()) {
        _searchBar->hide();
        _replaceBar->hide();
        return;
    }

    _searchBar->show();
    _replaceBar->show();

    // if text is selected, copy to FIND lineEdit
    QString sel = textEdit()->textCursor().selectedText();
    if (!sel.isEmpty()) {
        _searchBar->_findLineEdit->setText(sel);
    }

    _replaceBar->setFocus();
}


void MainView::hideReplaceBar()
{
    _replaceBar->hide();
}


bool MainView::isReplaceBarActive()
{
    return _replaceBar->isVisible();
}


void MainView::find(const QString & search, bool forward, bool casesensitive)
{
    QTextDocument::FindFlags flags;

    if (!forward) {
        flags |= QTextDocument::FindBackward;
    }

    if (casesensitive) {
        flags |= QTextDocument::FindCaseSensitively;
    }

    bool res = _textEdit->find(search, flags);
    if (!res)
        emit searchMessage("not found");
}


void MainView::replace(const QString &replace, bool justNext)
{
    QString find = _searchBar->_findLineEdit->text();
    bool matchCase = _searchBar->_caseCheckBox->isChecked();

    if (find.isEmpty()) {
        return;
    }

    Qt::CaseSensitivity cs = matchCase ? Qt::CaseSensitive : Qt::CaseInsensitive;
    QString content = _textEdit->toPlainText();

    if (!justNext) {
        content.replace(find, replace, cs);
        _textEdit->setPlainText(content);
        _textEdit->document()->setModified(true);
        return;
    }

    QTextDocument::FindFlags flags;

    if (matchCase) {
        flags |= QTextDocument::FindCaseSensitively;
    }

    bool found = _textEdit->find(find, flags);
    if (!found) {
        // TODO: advise search restart from the beginning...
        QTextCursor cur = _textEdit->textCursor();
        cur.setPosition(0);
        _textEdit->setTextCursor(cur);
        found = _textEdit->find(find,flags);
        if (!found) {
            emit searchMessage("not found");
            return;
        }
    }
    int n = find.length();
    int position = _textEdit->textCursor().position() - n;
    content.replace(position, n, replace);
    _textEdit->setPlainText(content);
    _textEdit->document()->setModified(true);
    QTextCursor cur = _textEdit->textCursor();
    cur.setPosition(position + n);
    _textEdit->setTextCursor(cur);
    return;
}
