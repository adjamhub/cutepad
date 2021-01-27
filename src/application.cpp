/*
 * Copyright (C) Andrea Diamantini 2020-2021 <adjam@protonmail.com>
 *
 * CutePad project
 *
 * @license GPL-3.0 <https://www.gnu.org/licenses/gpl-3.0.txt>
 */


#include "application.h"
#include "mainwindow.h"
#include "cutepadadaptor.h"

#include <QCommandLineParser>

#include <QDBusConnection>
#include <QDBusAbstractAdaptor>


Application::Application(int &argc, char *argv[])
    : QApplication(argc,argv)
{
    new CutepadAdaptor(this);
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

    const QStringList posArgs = parser.positionalArguments();
    loadPaths(posArgs);
}


void Application::loadPaths(const QStringList& paths)
{
    MainWindow *mainWin = nullptr;
    for (const QString &file : paths) {
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
