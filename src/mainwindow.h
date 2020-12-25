/*
 * Copyright (C) Andrea Diamantini 2020 <adjam@protonmail.com>
 *
 * CutePad project
 *
 * @license GPL-3.0 <https://www.gnu.org/licenses/gpl-3.0.txt>
 */
 

#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>

#include "mainview.h"

class QCloseEvent;
class QKeyEvent;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    // load & save settings automagically
    void loadSettings();
    void saveSettings();

    // needed to position next windows
    void tile(const QMainWindow *previous);

    // public functions to load and save the actual file
    // from the outside
    void loadFilePath(const QString & path);
    void saveFilePath(const QString & path);
    
    // ask user to save or not, eventually blocking exit action
    // returns true if window has to be closed, false otherwise
    bool exitAfterSaving();

protected:
    void closeEvent(QCloseEvent *event) override;
    void keyPressEvent(QKeyEvent *event);

private:
    void setupActions();

    void setCurrentFilePath(const QString& path);

private slots:
    void newWindow();
    void openFile();
    void saveFile();
    void saveFileAs();
    void closeFile();
    void printFile();

    void onZoomIn();
    void onZoomOut();
    void onZoomOriginal();
    void onFullscreen(bool on);

    void selectFont();
    void resetSettings();

    void about();

private:
    MainView* _view;

    QString _filePath;
    int _zoomRange;
};

#endif // MAINWINDOW_H
