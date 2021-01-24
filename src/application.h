/*
 * Copyright (C) Andrea Diamantini 2020 <adjam@protonmail.com>
 *
 * CutePad project
 *
 * @license GPL-3.0 <https://www.gnu.org/licenses/gpl-3.0.txt>
 */


#ifndef APPLICATION_H
#define APPLICATION_H


#include <QApplication>
#include <QStringList>


class Application : public QApplication
{
    Q_OBJECT

public:
    Application(int &argc, char *argv[]);
    ~Application();

    void parseCommandlineArgs();
    void loadPaths(const QStringList& paths);
};

#endif // APPLICATION_H
