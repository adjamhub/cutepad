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
#include <QLabel>
#include <QPushButton>


SearchBar::SearchBar(QWidget *parent)
    : QWidget(parent)
    , _lineEdit( new QLineEdit(this) )
{
	connect(_lineEdit, &QLineEdit::returnPressed, this, &SearchBar::findForward);
	
	auto label = new QLabel("Search for:", this);
	
	auto prevButton = new QPushButton("Previous", this);
	prevButton->setShortcut(QKeySequence::FindPrevious);
	connect(prevButton, &QPushButton::clicked, this, &SearchBar::findBackward);
	 
	auto nextButton = new QPushButton("Next", this);
	nextButton->setShortcut(QKeySequence::FindNext);
	connect(nextButton, &QPushButton::clicked, this, &SearchBar::findForward);
	
	auto caseCheckBox = new QCheckBox("Match Case", this);
	auto wholeWordCheckBox = new QCheckBox("Whole words", this);
	
	// The UI
    auto layout = new QHBoxLayout;
    layout->setContentsMargins (0, 0, 0, 0);
    layout->addWidget (label);
    layout->addWidget (_lineEdit);
    layout->addWidget (nextButton);
    layout->addWidget (prevButton);
    layout->addWidget (caseCheckBox);
    layout->addWidget (wholeWordCheckBox);
    layout->addStretch();
    setLayout (layout);
	    
    setFocusProxy(_lineEdit);
}


void SearchBar::findBackward()
{
//	void find(bool forward = true, bool casesensitive = false, bool wholewords = false);
	emit find(false);
}


void SearchBar::findForward()
{
	emit find(true);
}
