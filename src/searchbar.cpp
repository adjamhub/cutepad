/*
 * Copyright (C) Andrea Diamantini 2020 <adjam@protonmail.com>
 *
 * CutePad project
 *
 * @license GPL-3.0 <https://www.gnu.org/licenses/gpl-3.0.txt>
 */
 
 
#include "searchbar.h"

#include <QCheckBox>
#include <QHBoxLayout>
#include <QPushButton>


SearchBar::SearchBar(QWidget *parent)
    : QWidget(parent)
    , _lineEdit( new QLineEdit(this) )
{
	auto prevButton = new QPushButton("Previous", this);
	auto nextButton = new QPushButton("Next", this);
	auto caseCheckBox = new QCheckBox("Match Case", this);
	auto wholeWordCheckBox = new QCheckBox("Whole words", this);
	
	// The UI
    auto layout = new QHBoxLayout;
    layout->setContentsMargins (0, 0, 0, 0);
    layout->addWidget (_lineEdit);
    layout->addWidget (nextButton);
    layout->addWidget (prevButton);
    layout->addWidget (caseCheckBox);
    layout->addWidget (wholeWordCheckBox);
    layout->addStretch();
    setLayout (layout);
    
    setFocusProxy(_lineEdit);
}

