/*
 * Copyright (C) Andrea Diamantini 2020 <adjam@protonmail.com>
 *
 * CutePad project
 *
 * @license GPL-3.0 <https://www.gnu.org/licenses/gpl-3.0.txt>
 */


#include "replacebar.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>


ReplaceBar::ReplaceBar(QWidget *parent)
    : QWidget(parent)
    , _replaceLineEdit( new QLineEdit(this) )
{
    auto replaceLabel = new QLabel("Replace with:", this);
    replaceLabel->setMinimumWidth(100);

    _replaceLineEdit->setMinimumWidth(250);
    _replaceLineEdit->setMaximumWidth(250);

    auto replaceNextButton = new QPushButton("Replace", this);
    replaceNextButton->setMinimumWidth(120);
    connect(replaceNextButton, &QPushButton::clicked, this, &ReplaceBar::replaceNext);

    auto replaceAllButton = new QPushButton("Replace All", this);
    replaceAllButton->setMinimumWidth(120);
    connect(replaceAllButton, &QPushButton::clicked, this, &ReplaceBar::replaceAll);

    // The UI
    auto layout = new QHBoxLayout;
    layout->setContentsMargins (0, 0, 0, 0);
    layout->addWidget (replaceLabel);
    layout->addWidget (_replaceLineEdit);
    layout->addWidget (replaceNextButton);
    layout->addWidget (replaceAllButton);
    layout->addStretch();
    setLayout (layout);

    setFocusProxy(_replaceLineEdit);

    // tab order
    setTabOrder(_replaceLineEdit, replaceNextButton);
    setTabOrder(replaceNextButton, replaceAllButton);
}


void ReplaceBar::replaceNext()
{
    QString rep = _replaceLineEdit->text();
    emit replace(rep, true);
}


void ReplaceBar::replaceAll()
{
    QString rep = _replaceLineEdit->text();
    emit replace(rep, false);
}
