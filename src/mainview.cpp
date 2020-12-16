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


MainView::MainView (QWidget *parent)
    : QWidget (parent)
    , _textEdit(new TextEdit(this))
    , _searchbar(new Searchbar(this))
    , _replaceBar(new ReplaceBar(this))
    , _highlighter(new KSyntaxHighlighting::SyntaxHighlighter(_textEdit->document()))
    , _highlightRepo(new KSyntaxHighlighting::Repository)
{
	// The UI
    QGridLayout *mainGrid = new QGridLayout;
    mainGrid->setVerticalSpacing (4);
    mainGrid->setContentsMargins (0, 0, 0, 0);
    mainGrid->addWidget (_textEdit, 0, 0);
    mainGrid->addWidget (_searchbar, 1, 0);
    mainGrid->addWidget (_replaceBar, 2, 0);
    setLayout (mainGrid);

	// let's start with the hidden bar(s)
	_searchbar->setVisible(false);
	_replaceBar->setVisible(false);
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
	if (_searchbar->isVisible())
	{
		if (_searchbar->hasFocus())
		{
			_searchbar->hide();
			return;
		}
		_searchbar->setFocus();
		return;
	}
	_searchbar->show();
	_searchbar->setFocus();
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


void MainView::showLineNumbers(bool on)
{
	_textEdit->showLineNumbers(on);
}


void MainView::enableCurrentLineHighlighting(bool on)
{
	_textEdit->enableCurrentLineHighlighting(on);
}
