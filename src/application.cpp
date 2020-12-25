/*
 * Copyright (C) Andrea Diamantini 2020 <adjam@protonmail.com>
 *
 * CutePad project
 *
 * @license GPL-3.0 <https://www.gnu.org/licenses/gpl-3.0.txt>
 */


#include "version.h"

#include "application.h"
#include "mainwindow.h"

#include <QCommandLineParser>


Application::Application(int &argc, char *argv[])
    : QApplication(argc,argv)
{
}


Application::~Application()
{
}


void Application::parseCommandlineArgs()
{
    QCommandLineParser parser;
    parser.setApplicationDescription(QCoreApplication::applicationName());
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("file", "The file(s) to open.");
    parser.process(*this);

    MainWindow *mainWin = nullptr;
    const QStringList posArgs = parser.positionalArguments();
    for (const QString &file : posArgs) {
        MainWindow *newWin = new MainWindow;
        newWin->tile(mainWin);
        newWin->show();
        newWin->loadFilePath(file);
        mainWin = newWin;
    }

    if (!mainWin)
        mainWin = new MainWindow;
    mainWin->show();
}
