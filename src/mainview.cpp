/*
 * Copyright (C) Andrea Diamantini 2020 <adjam@protonmail.com>
 *
 * CutePad project
 *
 * @license GPL-3.0+ <https://spdx.org/licenses/GPL-3.0+.html>
 */


#include "mainview.h"

#include <QGridLayout>

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
    QGridLayout *mainGrid = new QGridLayout;
    mainGrid->setVerticalSpacing (4);
    mainGrid->setContentsMargins (0, 0, 0, 0);
    mainGrid->addWidget (_textEdit, 0, 0);
    mainGrid->addWidget (_searchBar, 1, 0);
    mainGrid->addWidget (_replaceBar, 2, 0);
    setLayout (mainGrid);

    // let's start with the hidden bar(s)
    _searchBar->setVisible(false);
    _replaceBar->setVisible(false); 
    
    connect(_searchBar, &SearchBar::find, this, &MainView::find);
    connect(this, &MainView::notFound, _searchBar, &SearchBar::notFoundMessage);
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
    // TODO: if text is selected, copy to lineEdit
    _searchBar->show();
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
    // TODO: if text is selected, copy to FIND lineEdit
    _replaceBar->show();
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


void MainView::find(bool forward, bool casesensitive, bool wholewords)
{
    QString search = _searchBar->lineEdit()->text();
    
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
