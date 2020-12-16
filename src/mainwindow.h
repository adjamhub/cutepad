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
#include <QAction>
#include <QToolBar>

#include "mainview.h"

class QCloseEvent;


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

private:
    void setupActions();
    void setupMenus();
    void setupToolbar();
    
    void setCurrentFilePath(const QString& path);
    
private slots:
    void documentWasModified();
    
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

    void about();
    
private:
    MainView* _view;
    
    QString _filePath;
    int _zoomRange;

	QToolBar* _mainToolbar;
	    
    QAction* _actionNew;
    QAction* _actionOpen;
    QAction* _actionSave;
    QAction* _actionSaveAs;
    QAction* _actionPrint;
    QAction* _actionClose;
    QAction* _actionExit;
    
    QAction* _actionUndo;
    QAction* _actionRedo;
    QAction* _actionCut;
    QAction* _actionCopy;
    QAction* _actionPaste;
    QAction* _actionSelectAll;
    
    QAction* _actionZoomIn;
    QAction* _actionZoomOut;
    QAction* _actionZoomOriginal;
    QAction* _actionFullScreen;
    
    QAction* _actionLineNumbers;
    QAction* _actionFind;
    QAction* _actionReplace;
    
    QAction* _actionAboutQt;
    QAction* _actionAboutApp;
};

#endif // MAINWINDOW_H
