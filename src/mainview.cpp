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
}


void MainView::showSearchbar()
{
	if (_searchBar->isVisible())
	{
		if (_searchBar->hasFocus())
		{
			_searchBar->hide();
			return;
		}
		_searchBar->setFocus();
		return;
	}
	_searchBar->show();
	_searchBar->setFocus();
}


void MainView::showReplaceBar()
{
	if (_replaceBar->isVisible())
	{
		_replaceBar->hide();
		return;
	}
	_replaceBar->show();
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
