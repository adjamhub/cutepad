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
#include <QTextCodec>

#include "textedit.h"
#include "searchbar.h"
#include "replacebar.h"


class MainView : public QWidget
{
    Q_OBJECT

public:
    MainView (QWidget *parent = nullptr);

    TextEdit* textEdit() {
        return _textEdit;
    }

    bool isSearchBarActive();
    bool isReplaceBarActive();

    QTextCodec* textCodec();
    void setTextCodec(QTextCodec* codec);

public slots:
    void showSearchBar();
    void hideSearchBar();

    void showReplaceBar();
    void hideReplaceBar();

private slots:
    void find(const QString & search,
              bool forward = true,
              bool casesensitive = false);

    void replace(const QString &replace, bool justNext = true);

signals:
    void searchMessage(const QString &);

private:
    TextEdit* _textEdit;
    SearchBar* _searchBar;
    ReplaceBar* _replaceBar;

    QTextCodec* _textCodec;
};


#endif // MAIN_VIEW_H
