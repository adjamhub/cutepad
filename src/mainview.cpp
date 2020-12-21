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
    connect(this, &MainView::notFound, _searchBar, &SearchBar::notFoundMessage);

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

    // TODO: if text is selected, copy to lineEdit
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

    // TODO: if text is selected, copy to FIND lineEdit
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


void MainView::find(const QString & search, bool forward, bool casesensitive, bool wholewords)
{
    QTextDocument::FindFlags flags;

    if (!forward) {
        flags |= QTextDocument::FindBackward;
    }

    if (casesensitive) {
        flags |= QTextDocument::FindCaseSensitively;
    }

    if (wholewords) {
        flags |= QTextDocument::FindWholeWords;
    }

    bool res = _textEdit->find(search, flags);
    if (!res)
        emit notFound();
}


void MainView::replace(const QString &replace, bool justNext)
{
    QString find = _searchBar->_findLineEdit->text();
    bool matchCase = _searchBar->_caseCheckBox->isChecked();
    bool wholeWords = _searchBar->_wholeWordCheckBox->isChecked();

    qDebug() << "find: " << find;
    qDebug() << "replace: " << replace;
    qDebug() << "just next: " << justNext;
    qDebug() << "match case: " << matchCase;
    qDebug() << "whole words: " << wholeWords;
}


void MainView::highlightText(const QString &text)
{
    if (text.isEmpty())
        return;

    // TODO ...
}
