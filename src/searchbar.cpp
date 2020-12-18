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
    , _caseCheckBox( new QCheckBox("Match Case", this) )
    , _wholeWordCheckBox( new QCheckBox("Whole words", this) )
    , _notFoundLabel( new QLabel(this) )
{
	connect(_lineEdit, &QLineEdit::returnPressed, this, &SearchBar::findForward);
	
	auto label = new QLabel("Search for:", this);
	
	auto prevButton = new QPushButton("Previous", this);
	prevButton->setShortcut(QKeySequence::FindPrevious);
	connect(prevButton, &QPushButton::clicked, this, &SearchBar::findBackward);
	 
	auto nextButton = new QPushButton("Next", this);
	nextButton->setShortcut(QKeySequence::FindNext);
	connect(nextButton, &QPushButton::clicked, this, &SearchBar::findForward);
	
	// The UI
    auto layout = new QHBoxLayout;
    layout->setContentsMargins (0, 0, 0, 0);
    layout->addWidget (label);
    layout->addWidget (_lineEdit);
    layout->addWidget (nextButton);
    layout->addWidget (prevButton);
    layout->addWidget (_caseCheckBox);
    layout->addWidget (_wholeWordCheckBox);
    layout->addStretch();
    layout->addWidget (_notFoundLabel);
    layout->addStretch();
    
    setLayout (layout);
	    
    setFocusProxy(_lineEdit);
}


void SearchBar::findBackward()
{
	_notFoundLabel->setText("");

	bool caseSensitive = _caseCheckBox->isChecked();
	bool wholeWords = _wholeWordCheckBox->isChecked();
	emit find(false, caseSensitive, wholeWords);
}


void SearchBar::findForward()
{
	_notFoundLabel->setText("");

	bool caseSensitive = _caseCheckBox->isChecked();
	bool wholeWords = _wholeWordCheckBox->isChecked();
	emit find(true, caseSensitive, wholeWords);
}


void SearchBar::notFoundMessage()
{
	_notFoundLabel->setText("<b>Not Found</b>");
}
