/*
 * Copyright (C) Andrea Diamantini 2020 <adjam@protonmail.com>
 *
 * CutePad project
 *
 * @license GPL-3.0 <https://www.gnu.org/licenses/gpl-3.0.txt>
 */
 

#ifndef REPLACEBAR_H
#define REPLACEBAR_H


#include <QWidget>
#include <QLineEdit>


class ReplaceBar : public QWidget
{
    Q_OBJECT

public:
    ReplaceBar(QWidget *parent = nullptr);
    
private:
    QLineEdit* _findLineEdit;
    QLineEdit* _replaceLineEdit;
};

#endif // SEARCHBAR_H
