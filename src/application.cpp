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


Application *Application::instance()
{
    return (qobject_cast<Application *>(QCoreApplication::instance()));
}


void Application::removeWindowFromList(MainWindow* w)
{
    _windows.removeOne(w);
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
    if (paths.isEmpty()) {
        loadPath("");
        return;
    }

    for (const QString &file : paths) {
        loadPath(file);
    }
}


void Application::loadPath(const QString& path)
{
    if (path.isEmpty()) {
        MainWindow *mainWin = new MainWindow;
        _windows.append(mainWin);
        mainWin->show();
        return;
    }

    for (MainWindow* win : qAsConst(_windows)) {
        if (win->filePath() == path) {
            win->setFocus();
            return;
        }
    }

    MainWindow *mainWin = new MainWindow;
    _windows.append(mainWin);
    mainWin->show();
    mainWin->loadFilePath(path);
}


void Application::loadSettings()
{
    for (MainWindow* win : qAsConst(_windows)) {
        win->loadSettings();
    }
}
