/*
 * Copyright (C) Andrea Diamantini 2020 <adjam@protonmail.com>
 *
 * CutePad project
 *
 * @license GPL-3.0 <https://www.gnu.org/licenses/gpl-3.0.txt>
 */
 

#include "mainwindow.h"

#include <QApplication>
#include <QCloseEvent>
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QScreen>
#include <QStandardPaths>
#include <QTextStream>
#include <QToolBar>

#include <QDebug>

#include <QPrinter>
#include <QPrintDialog>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , _view(new MainView(this))
    , _filePath("")
{
    setCentralWidget(_view);

    setupActions();
    setupMenus();
    setupToolbar();

    connect(_view->textEdit()->document(), &QTextDocument::contentsChanged,this, &MainWindow::documentWasModified);
     
    // application icon and title
    QIcon appIcon(":/cutepad.png");
    setWindowIcon(appIcon);
    setWindowTitle("untitled - cutepad");

    _actionCut->setEnabled(false);
    _actionCopy->setEnabled(false);
    _actionUndo->setEnabled(false);
    _actionRedo->setEnabled(false);    
    connect(_view->textEdit(), &QPlainTextEdit::copyAvailable, _actionCut, &QAction::setEnabled);
    connect(_view->textEdit(), &QPlainTextEdit::copyAvailable, _actionCopy, &QAction::setEnabled);
    connect(_view->textEdit(), &QPlainTextEdit::undoAvailable, _actionUndo, &QAction::setEnabled);
    connect(_view->textEdit(), &QPlainTextEdit::redoAvailable, _actionRedo, &QAction::setEnabled);
}


void MainWindow::tile(const QMainWindow *previous)
{
    if (!previous)
        return;
    int topFrameWidth = previous->geometry().top() - previous->pos().y();
    if (!topFrameWidth)
        topFrameWidth = 40;
    const QPoint pos = previous->pos() + 2 * QPoint(topFrameWidth, topFrameWidth);
    if (screen()->availableGeometry().contains(rect().bottomRight() + pos))
        move(pos);
}


void MainWindow::loadFilePath(const QString &path)
{
    _view->loadFilePath(path);
    _filePath = path;
}


void MainWindow::saveFilePath(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QString content = _view->textEdit()->toPlainText();
    QTextStream out(&file);
    out << content;

    // set "File Name - cutepad" on title bar
    QFileInfo info(file);
    QString fName = info.fileName();
    setWindowTitle(fName + " - cutepad");

    _filePath = path;
}


bool MainWindow::exitAfterSaving()
{
    if (isWindowModified())
    {
        int risp = QMessageBox::question(this,
                                         "Save Changes",
                                         "The file has unsaved changes",
                                         QMessageBox::Save | QMessageBox::No | QMessageBox::Cancel);

        switch(risp)
        {
        case QMessageBox::Save:
            // save and exit
            saveFile();
            break;

        case QMessageBox::No:
            // don't save and exit
            break;

        case QMessageBox::Cancel:
            // don't exit
            return false;

        default:
            // this should never happen
            break;
        }
    }
    return true;
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    if (exitAfterSaving())
    {
        event->accept();
        return;
    }
    event->ignore();
}


void MainWindow::setupActions()
{
    _actionNew = new QAction( QIcon::fromTheme("document-new") , "New", this);
    connect(_actionNew,&QAction::triggered, this, &MainWindow::newWindow);      
    _actionOpen = new QAction( QIcon::fromTheme("document-open"), "Open", this);
    connect(_actionOpen,&QAction::triggered, this, &MainWindow::openFile);  
    _actionSave = new QAction( QIcon::fromTheme("document-save"), "Save", this);
    connect(_actionSave,&QAction::triggered, this, &MainWindow::saveFile);
    _actionSaveAs = new QAction( QIcon::fromTheme("document-save-as"), "Save As", this);
    connect(_actionSaveAs,&QAction::triggered, this, &MainWindow::saveFileAs);  
    _actionPrint = new QAction( QIcon::fromTheme("document-print"), "Print", this);
    connect(_actionPrint,&QAction::triggered, this, &MainWindow::printFile);    
    _actionClose = new QAction( QIcon::fromTheme("document-close"), "Close", this);
    connect(_actionClose,&QAction::triggered, this, &MainWindow::closeFile);    
    _actionExit = new QAction( QIcon::fromTheme("application-exit"), "Exit", this );
    connect(_actionExit, &QAction::triggered, qApp, &QApplication::quit);
    
    _actionUndo = new QAction( QIcon::fromTheme("edit-undo"), "Undo", this );
    connect(_actionUndo,&QAction::triggered, _view->textEdit(), &TextEdit::undo );  
    _actionRedo = new QAction(QIcon::fromTheme("edit-redo") , "Redo", this);
    connect(_actionRedo,&QAction::triggered, _view->textEdit(), &TextEdit::redo );  
    _actionCut = new QAction(QIcon::fromTheme("edit-cut"), "Cut", this );
    connect(_actionCut,&QAction::triggered, _view->textEdit(), &TextEdit::cut );    
    _actionCopy = new QAction(QIcon::fromTheme("edit-copy"), "Copy", this );
    connect(_actionCopy,&QAction::triggered, _view->textEdit(), &TextEdit::copy );  
    _actionPaste = new QAction(QIcon::fromTheme("edit-paste"), "Paste", this );
    connect(_actionPaste,&QAction::triggered,  _view->textEdit(), &TextEdit::paste );   
    _actionSelectAll = new QAction(QIcon::fromTheme("edit-select-all"), "Select All", this );
    connect(_actionSelectAll,&QAction::triggered, _view->textEdit(), &TextEdit::selectAll );
    
    _actionZoomIn = new QAction( QIcon::fromTheme("zoom-in"), "Zoom In", this );
    connect(_actionZoomIn, &QAction::triggered, this, &MainWindow::onZoomIn );  
    _actionZoomOut = new QAction( QIcon::fromTheme("zoom-out"), "Zoom Out", this );
    connect(_actionZoomIn, &QAction::triggered, this, &MainWindow::onZoomOut ); 
    _actionZoomOriginal = new QAction( QIcon::fromTheme("zoom-original"), "Zoom Original", this );
    connect(_actionZoomIn, &QAction::triggered, this, &MainWindow::onZoomOriginal );    
    _actionFullScreen = new QAction( QIcon::fromTheme("view-fullscreen"), "Full Screen", this );
    _actionFullScreen->setCheckable(true);
    connect(_actionFullScreen, &QAction::triggered, this, &MainWindow::onFullscreen );  

    _actionLineNumbers = new QAction( "Line Numbers", this );
    _actionLineNumbers->setCheckable(true);
    connect(_actionLineNumbers, &QAction::triggered, _view, &MainView::showLineNumbers );   
    _actionFind = new QAction( QIcon::fromTheme("edit-find"), "Find", this );
    _actionFind->setCheckable(true);
    connect(_actionFind, &QAction::triggered, _view, &MainView::setSearchbarVisible );  
    _actionReplace = new QAction( QIcon::fromTheme("edit-replace"), "Replace", this );
    _actionReplace->setCheckable(true);
    //connect(_actionReplace, &QAction::triggered, this, &MainWindow::boh );    
    
    _actionAboutQt = new QAction("About Qt", this );
    connect(_actionAboutQt,&QAction::triggered, qApp, &QApplication::aboutQt);  
    _actionAboutApp = new QAction("About", this );
    connect(_actionAboutApp, &QAction::triggered, this, &MainWindow::about);    
}


void MainWindow::setupMenus()
{
    QMenu* fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction(_actionNew);
    fileMenu->addAction(_actionOpen);
    fileMenu->addAction(_actionSave);   
    fileMenu->addAction(_actionSaveAs); 
    fileMenu->addSeparator();
    fileMenu->addAction(_actionPrint);  
    fileMenu->addSeparator();
    fileMenu->addAction(_actionClose);  
    fileMenu->addAction(_actionExit);   

    QMenu* editMenu = menuBar()->addMenu("&Edit");
    editMenu->addAction(_actionUndo);
    editMenu->addAction(_actionRedo);
    editMenu->addSeparator();
    editMenu->addAction(_actionCut);
    editMenu->addAction(_actionCopy);
    editMenu->addAction(_actionPaste);
    editMenu->addSeparator();
    editMenu->addAction(_actionSelectAll);
    
    QMenu* viewMenu = menuBar()->addMenu("&View");
    viewMenu->addAction(_actionZoomIn);
    viewMenu->addAction(_actionZoomOut);
    viewMenu->addAction(_actionZoomOriginal);
    viewMenu->addSeparator();
    viewMenu->addAction(_actionFullScreen);

    QMenu* optionsMenu = menuBar()->addMenu("&Options");
    optionsMenu->addAction(_actionLineNumbers);
    optionsMenu->addAction(_actionFind);
    optionsMenu->addAction(_actionReplace);
    
    QMenu* helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction(_actionAboutQt);
    helpMenu->addAction(_actionAboutApp);   
}


void MainWindow::setupToolbar()
{
    QToolBar* toolBar = addToolBar("Main Toolbar");
    toolBar->addAction(_actionNew);
    toolBar->addAction(_actionOpen);
    toolBar->addAction(_actionSave);
    toolBar->addSeparator();
    toolBar->addAction(_actionPrint);
    toolBar->addSeparator();
    toolBar->addAction(_actionUndo);
    toolBar->addAction(_actionRedo);
    toolBar->addSeparator();
    toolBar->addAction(_actionCut);
    toolBar->addAction(_actionCopy);
    toolBar->addAction(_actionPaste);

    // toolbar style and (position) lock
    toolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toolBar->setMovable(false);
}


void MainWindow::documentWasModified()
{
	bool needToSave = _view->textEdit()->document()->isModified();
	setWindowModified(needToSave);
	_actionSave->setEnabled(needToSave);
}


void MainWindow::newWindow()
{
	MainWindow *other = new MainWindow;
    other->tile(this);
    other->show();
}


void MainWindow::openFile()
{
    QString path = QFileDialog::getOpenFileName(this);
    if (path.isEmpty())
        return;
    loadFilePath(path);
}


void MainWindow::saveFile()
{
    if (_filePath.isEmpty())
    {
        saveFileAs();
        return;
    }

    saveFilePath(_filePath);
}


void MainWindow::saveFileAs()
{
    // needed to catch document dir location (and it has to be writable, obviously...)
    QString documentDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString path = QFileDialog::getSaveFileName(this,"Save File", documentDir);
    if (path.isEmpty())
        return;

    // try to add .txt extension in the end
    QFileInfo info(path);
    if (info.fileName() == info.baseName())
    {
        path += ".txt";
    }

    saveFilePath(path);
}


void MainWindow::closeFile()
{
    if (exitAfterSaving())
    {
        // clear() triggers PlainTextEdit textChange signal...
        _view->textEdit()->clear();
        setWindowModified(false);
        setWindowTitle("untitled - cutepad");
        return;
    }
}


void MainWindow::printFile()
{
    QPrinter printer;
    QPrintDialog printDialog(&printer, this);
    if (printDialog.exec() == QDialog::Accepted)
    {
        _view->textEdit()->print(&printer);
    }
}


void MainWindow::onZoomIn()
{
}


void MainWindow::onZoomOut()
{
}


void MainWindow::onZoomOriginal()
{
}


void MainWindow::onFullscreen(bool on)
{
    if (on)
    {
        showFullScreen();
        //ui->toolBar->addAction(_actionFullScreen);
        _actionFullScreen->setText("Exit FullScreen");
        //ui->menubar->hide();
    }
    else
    {
        showNormal();
        //ui->toolBar->removeAction(_actionFullScreen);
        _actionFullScreen->setText("FullScreen");
        //ui->menubar->show();
    }
}


void MainWindow::about()
{
    QString version = qApp->applicationVersion();
    
    QMessageBox::about(this, 
                       "About cutepad",
                        "cutepad " + version + "\n\nThe Qt pad ;)\nJust an easy plain text editor, based on Qt libraries");
}
