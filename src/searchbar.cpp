/*
 * Copyright (C) Andrea Diamantini 2020 <adjam@protonmail.com>
 *
 * CutePad project
 *
 * @license GPL-3.0 <https://www.gnu.org/licenses/gpl-3.0.txt>
 */
 
 
#include "searchbar.h"

#include <QHBoxLayout>

Searchbar::Searchbar(QWidget *parent)
    : QWidget(parent)
    , _lineEdit( new QLineEdit(this) )
    , _searchButton( new QPushButton(this) )
{
	_searchButton->setText("Find");
	
	// The UI
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setContentsMargins (0, 0, 0, 0);
    layout->addWidget (_lineEdit);
    layout->addWidget (_searchButton);
    setLayout (layout);
    
    setFocusProxy(_lineEdit);
}

