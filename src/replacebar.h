/*
 * Copyright (C) Andrea Diamantini 2020-2021 <adjam@protonmail.com>
 *
 * CutePad project
 *
 * @license GPL-3.0 <https://www.gnu.org/licenses/gpl-3.0.txt>
 */


#ifndef REPLACEBAR_H
#define REPLACEBAR_H


#include <QWidget>
#include <QLineEdit>


class MainView;


class ReplaceBar : public QWidget
{
    Q_OBJECT

public:
    ReplaceBar(QWidget *parent = nullptr);

signals:
    void replace(const QString &replace, bool justNext = true);

private slots:
    void replaceNext();
    void replaceAll();

private:
    friend class MainView;

    QLineEdit* _replaceLineEdit;
};

#endif // SEARCHBAR_H
