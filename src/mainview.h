/*
 * Copyright (C) Pedram Pourang (aka Tsu Jan) 2014-2020 <tsujan2000@gmail.com>
 * Copyright (C) Andrea Diamantini 2020 <adjam@protonmail.com>
 *
 * CutePad project
 *
 * @license GPL-3.0+ <https://spdx.org/licenses/GPL-3.0+.html>
 */


#ifndef MAIN_VIEW_H
#define MAIN_VIEW_H


#include <QWidget>


#include "textedit.h"
#include "searchbar.h"

#include <KSyntaxHighlighting/Repository>
#include <KSyntaxHighlighting/SyntaxHighlighter>


class MainView : public QWidget
{
    Q_OBJECT
public:
    MainView (QWidget *parent = nullptr);
    
    TextEdit* textEdit() const {
    	return _textEdit;
    }

	// load file content and enable syntax highlighting
    void loadFilePath(const QString & path);

public slots:    
    void setSearchbarVisible(bool on);
    void showLineNumbers(bool on);
    
private:
	TextEdit* _textEdit;
	Searchbar* _searchbar;

	KSyntaxHighlighting::SyntaxHighlighter* _highlighter;
	KSyntaxHighlighting::Repository* _highlightRepo;
};


#endif // MAIN_VIEW_H

