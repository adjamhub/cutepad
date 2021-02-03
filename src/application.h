/*
 * Copyright (C) Andrea Diamantini 2020-2021 <adjam@protonmail.com>
 *
 * CutePad project
 *
 * @license GPL-3.0 <https://www.gnu.org/licenses/gpl-3.0.txt>
 */


#ifndef APPLICATION_H
#define APPLICATION_H


#include <QApplication>
#include <QFileSystemWatcher>
#include <QList>
#include <QStringList>

class MainWindow;


class Application : public QApplication
{
    Q_OBJECT

public:
    Application(int &argc, char *argv[]);
    ~Application();

    static Application* instance();

    void parseCommandlineArgs();

    void loadPaths(const QStringList& paths);
    void loadPath(const QString& path);

    void removeWindowFromList(MainWindow* w);

    void loadSettings();

    // File System Watcher
    void addWatchedPath(const QString& path);
    void removeWatchedPath(const QString& path);

private Q_SLOTS:
    void notifyFileChanged(const QString& path);

private:
    QList<MainWindow*> _windows;
    QFileSystemWatcher* _watcher;
};

#endif // APPLICATION_H
