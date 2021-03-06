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

class QLineEdit;
class MainWindow;


class ReplaceBar : public QWidget
{
    Q_OBJECT

public:
    explicit ReplaceBar(QWidget *parent = nullptr);

Q_SIGNALS:
    void replace(const QString &replace, bool justNext = true);

private Q_SLOTS:
    void replaceNext();
    void replaceAll();

private:
    friend class MainWindow;

    QLineEdit* _replaceLineEdit;
};

#endif // SEARCHBAR_H
