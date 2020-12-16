/*
 * Copyright (C) Andrea Diamantini 2020 <adjam@protonmail.com>
 *
 * CutePad project
 *
 * @license GPL-3.0 <https://www.gnu.org/licenses/gpl-3.0.txt>
 */
 

#ifndef SEARCHBAR_H
#define SEARCHBAR_H


#include <QWidget>
#include <QLineEdit>
#include <QPushButton>

class Searchbar : public QWidget
{
    Q_OBJECT

public:
    Searchbar(QWidget *parent = nullptr);
    
private:
    QLineEdit* _lineEdit;
    QPushButton* _searchButton;
};

#endif // SEARCHBAR_H
