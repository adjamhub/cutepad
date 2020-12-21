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

#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>


class SearchBar : public QWidget
{
    Q_OBJECT

public:
    SearchBar(QWidget *parent = nullptr);

    QLineEdit* lineEdit() const {
        return _lineEdit;
    }

signals:
    void find(bool forward = true, bool casesensitive = false, bool wholewords = false);

public slots:
    void notFoundMessage();

private slots:
    void findBackward();
    void findForward();

private:
    QLineEdit* _lineEdit;

    QCheckBox* _caseCheckBox;
    QCheckBox* _wholeWordCheckBox;
    QLabel* _notFoundLabel;
};

#endif // SEARCHBAR_H
