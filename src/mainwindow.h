/*
 * Copyright (C) Andrea Diamantini 2020-2021 <adjam@protonmail.com>
 *
 * CutePad project
 *
 * @license GPL-3.0 <https://www.gnu.org/licenses/gpl-3.0.txt>
 */


#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>

#include "textedit.h"
#include "searchbar.h"
#include "replacebar.h"
#include "statusbar.h"

class QCloseEvent;
class QKeyEvent;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    inline QString filePath() const { return _filePath; }

    void loadSettings();

    // needed to position next windows
    void tile(const QMainWindow *previous);

    // public functions to load and save the actual file
    // from the outside
    void loadFilePath(const QString & path);
    void saveFilePath(const QString & path);
    
    // ask user to save or not, eventually blocking exit action
    // returns true if window has to be closed, false otherwise
    bool exitAfterSaving();

    // reload file modified OUTSIDE of cutepad management
    // (asking user before)
    void reloadChangedFile();

protected:
    void closeEvent(QCloseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    void setupActions();

    void setCurrentFilePath(const QString& path);
    void addPathToRecentFiles(const QString& path);

private slots:
    void newWindow();
    void openFile();
    void saveFile();
    void saveFileAs();
    void printFile();

    void onZoomIn();
    void onZoomOut();
    void onZoomOriginal();
    void onFullscreen(bool on);

    void showSettings();

    void about();
    void showManual();

    void updateStatusBar();
    void encode();

    void showSearchBar();
    void showReplaceBar();

    void search(const QString & search,
                bool forward = true,
                bool casesensitive = false);

    void replace(const QString &replace, bool justNext = true);

    void recentFileTriggered();

signals:
    void searchMessage(const QString &);

private:
    TextEdit* _textEdit;
    SearchBar* _searchBar;
    ReplaceBar* _replaceBar;
    StatusBar* _statusBar;

    QString _filePath;
    int _zoomRange;
    bool _canBeReloaded;
};

#endif // MAINWINDOW_H
