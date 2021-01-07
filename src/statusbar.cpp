/*
 * Copyright (C) Andrea Diamantini 2020 <adjam@protonmail.com>
 *
 * CutePad project
 *
 * @license GPL-3.0 <https://www.gnu.org/licenses/gpl-3.0.txt>
 */


#include "statusbar.h"

#include <QHBoxLayout>


StatusBar::StatusBar(QWidget *parent)
    : QWidget(parent)
    , _langLabel(new QLabel(this))
    , _posLabel(new QLabel(this))
    , _codecLabel(new QLabel(this))
{
    // The UI
    auto layout = new QHBoxLayout;
    layout->setContentsMargins (0, 0, 0, 0);
    layout->addWidget (_posLabel);
    layout->addWidget (_langLabel);
    layout->addWidget (_codecLabel);
    setLayout (layout);
}


void StatusBar::setLanguage(const QString& lang)
{
    QString msg;
    msg += "&nbsp;&nbsp;<b>Language: </b>";
    msg += lang;
    _langLabel->setText(msg);
}


void StatusBar::setPosition(int row, int col)
{
    QString msg;
    msg += "&nbsp;&nbsp;<b>Row: </b>";
    msg += QString::number(row + 1);
    msg += ", <b>Column: </b>";
    msg += QString::number(col + 1);
    _posLabel->setText(msg);
}


void StatusBar::setCodec(const QString& codec)
{
    QString msg;
    msg += "&nbsp;&nbsp;<b>Encoding: </b>";
    msg += codec;
    _codecLabel->setText(msg);
}

    