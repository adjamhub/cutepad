/*
 * Copyright (C) Andrea Diamantini 2020 <adjam@protonmail.com>
 *
 * CutePad project
 *
 * @license GPL-3.0 <https://www.gnu.org/licenses/gpl-3.0.txt>
 */


#include "replacebar.h"

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>


ReplaceBar::ReplaceBar(QWidget *parent)
    : QWidget(parent)
    , _findLineEdit( new QLineEdit(this) )
    , _replaceLineEdit( new QLineEdit(this) )
{
    auto findLabel = new QLabel("Find:", this);
    auto replaceLabel = new QLabel("Replace", this);
    auto replaceNextButton = new QPushButton("Replace", this);
    auto replaceAllButton = new QPushButton("Replace All", this);

    // The UI
    auto layout = new QGridLayout;
    layout->setContentsMargins (0, 0, 0, 0);
    layout->addWidget (findLabel, 0, 0);
    layout->addWidget (_findLineEdit, 0, 1);
    layout->addWidget (replaceNextButton, 0, 2);
    layout->addWidget (replaceLabel, 1, 0);
    layout->addWidget (_replaceLineEdit, 1, 1);
    layout->addWidget (replaceAllButton, 1, 2);
    setLayout (layout);
}

