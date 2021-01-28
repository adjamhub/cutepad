/*
 * Copyright (C) Andrea Diamantini 2020-2021 <adjam@protonmail.com>
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


class MainWindow;


class SearchBar : public QWidget
{
    Q_OBJECT

public:
    SearchBar(QWidget *parent);

signals:
    void search(const QString &search,
                bool forward = true,
                bool casesensitive = false);

public slots:
    void searchMessage(const QString & msg);

private slots:
    void findBackward();
    void findForward();

private:
    friend class MainWindow;

    QLineEdit* _findLineEdit;

    QCheckBox* _caseCheckBox;
    QLabel* _notFoundLabel;
};

#endif // SEARCHBAR_H
