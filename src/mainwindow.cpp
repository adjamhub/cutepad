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
#include <QSettings>
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
    setAttribute(Qt::WA_DeleteOnClose);
    setCentralWidget(_view);

    setupActions();
    setupMenus();
    setupToolbar();
     
    // application icon and title
    QIcon appIcon(":/cutepad.png");
    setWindowIcon(appIcon);
    
    loadSettings();
    
    setCurrentFilePath("");
    connect(_view->textEdit()->document(), &QTextDocument::contentsChanged,this, &MainWindow::documentWasModified); 
}


void MainWindow::loadSettings()
{
    QSettings s ( QCoreApplication::organizationName() , QCoreApplication::applicationName() );
    
    // size and position
    QSize size = s.value("size", QSize(800,600)).toSize();
    resize(size);
    QPoint pos = s.value("pos", QPoint(50,50)).toPoint();
    move(pos);

    // colors
    QPalette p = _view->textEdit()->palette();
    QColor textColor = QColor(s.value("textColor", "#000000").toString());
    QColor baseColor = QColor(s.value("baseColor", "#FFFFFF").toString());
    p.setColor(QPalette::Text, textColor);
    p.setColor(QPalette::Base, baseColor);
    _view->textEdit()->setPalette(p);

    // font
    QString fontFamily = s.value("fontFamily", "Noto Sans").toString();
    int fontSize = s.value("fontSize", 12).toInt();
    int fontWeight = s.value("fontWeight", 50).toInt();
    bool italic = s.value("fontItalic", false).toBool();
    QFont font(fontFamily,fontSize, fontWeight);
    font.setItalic(italic);
    _view->textEdit()->setFont(font);
}


void MainWindow::saveSettings()
{
    QSettings s ( QCoreApplication::organizationName() , QCoreApplication::applicationName() );

    // size and dimensions
    s.setValue("size", size() );
    s.setValue("pos", pos() );

    // colors
    QPalette p = _view->textEdit()->palette();
    QString textColor = p.color(QPalette::Text).name();
    QString baseColor = p.color(QPalette::Base).name();
    s.setValue("textColor", textColor);
    s.setValue("baseColor", baseColor);

    // font
    QFont f = _view->textEdit()->font();
    QString fontFamily = f.family();
    int fontSize = f.pointSize();
    int fontWeight = f.weight();
    bool italic = f.italic();
    s.setValue("fontFamily", fontFamily);
    s.setValue("fontSize", fontSize);
    s.setValue("fontWeight", fontWeight);
    s.setValue("fontItalic", italic);
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
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
    _view->textEdit()->setPlainText(file.readAll());
    QGuiApplication::restoreOverrideCursor();

    setCurrentFilePath(path);    
}


void MainWindow::saveFilePath(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QString content = _view->textEdit()->toPlainText();
    QTextStream out(&file);
    out << content;

    setCurrentFilePath(path);
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
        saveSettings();
        event->accept();
        return;
    }
    event->ignore();
}


void MainWindow::setupActions()
{
    _actionNew = new QAction( QIcon::fromTheme("document-new") , "New", this);
    _actionNew->setShortcut(QKeySequence::New);
    connect(_actionNew,&QAction::triggered, this, &MainWindow::newWindow);      
    _actionOpen = new QAction( QIcon::fromTheme("document-open"), "Open", this);
    _actionOpen->setShortcut(QKeySequence::Open);
    connect(_actionOpen,&QAction::triggered, this, &MainWindow::openFile);  
    _actionSave = new QAction( QIcon::fromTheme("document-save"), "Save", this);
    _actionSave->setShortcut(QKeySequence::Save);
    connect(_actionSave,&QAction::triggered, this, &MainWindow::saveFile);
    _actionSaveAs = new QAction( QIcon::fromTheme("document-save-as"), "Save As", this);
    connect(_actionSaveAs,&QAction::triggered, this, &MainWindow::saveFileAs);  
    _actionPrint = new QAction( QIcon::fromTheme("document-print"), "Print", this);
    _actionPrint->setShortcut(QKeySequence::Print);
    connect(_actionPrint,&QAction::triggered, this, &MainWindow::printFile);    
    _actionClose = new QAction( QIcon::fromTheme("document-close"), "Close", this);
    _actionClose->setShortcut(QKeySequence::Close);
    connect(_actionClose,&QAction::triggered, this, &MainWindow::closeFile);    
    _actionExit = new QAction( QIcon::fromTheme("application-exit"), "Exit", this );
    _actionExit->setShortcut(QKeySequence::Quit);
    connect(_actionExit, &QAction::triggered, qApp, &QApplication::quit);
    
    _actionUndo = new QAction( QIcon::fromTheme("edit-undo"), "Undo", this );
    _actionUndo->setShortcut(QKeySequence::Undo);
    connect(_actionUndo,&QAction::triggered, _view->textEdit(), &TextEdit::undo );  
    _actionRedo = new QAction(QIcon::fromTheme("edit-redo") , "Redo", this);
    _actionRedo->setShortcut(QKeySequence::Redo);
    connect(_actionRedo,&QAction::triggered, _view->textEdit(), &TextEdit::redo );  
    _actionCut = new QAction(QIcon::fromTheme("edit-cut"), "Cut", this );
    _actionCut->setShortcut(QKeySequence::Cut);
    connect(_actionCut,&QAction::triggered, _view->textEdit(), &TextEdit::cut );    
    _actionCopy = new QAction(QIcon::fromTheme("edit-copy"), "Copy", this );
    _actionCopy->setShortcut(QKeySequence::Copy);
    connect(_actionCopy,&QAction::triggered, _view->textEdit(), &TextEdit::copy );  
    _actionPaste = new QAction(QIcon::fromTheme("edit-paste"), "Paste", this );
    _actionPaste->setShortcut(QKeySequence::Paste);
    connect(_actionPaste,&QAction::triggered,  _view->textEdit(), &TextEdit::paste );   
    _actionSelectAll = new QAction(QIcon::fromTheme("edit-select-all"), "Select All", this );
    _actionSelectAll->setShortcut(QKeySequence::SelectAll);
    connect(_actionSelectAll,&QAction::triggered, _view->textEdit(), &TextEdit::selectAll );
    
    _actionZoomIn = new QAction( QIcon::fromTheme("zoom-in"), "Zoom In", this );
    _actionZoomIn->setShortcut(QKeySequence::ZoomIn);
    connect(_actionZoomIn, &QAction::triggered, this, &MainWindow::onZoomIn );  
    _actionZoomOut = new QAction( QIcon::fromTheme("zoom-out"), "Zoom Out", this );
    _actionZoomOut->setShortcut(QKeySequence::ZoomOut);
    connect(_actionZoomIn, &QAction::triggered, this, &MainWindow::onZoomOut ); 
    _actionZoomOriginal = new QAction( QIcon::fromTheme("zoom-original"), "Zoom Original", this );
    _actionZoomOriginal->setShortcut(Qt::CTRL + Qt::Key_0);
    connect(_actionZoomIn, &QAction::triggered, this, &MainWindow::onZoomOriginal );    
    _actionFullScreen = new QAction( QIcon::fromTheme("view-fullscreen"), "Full Screen", this );
    _actionFullScreen->setShortcuts(QKeySequence::FullScreen);
    _actionFullScreen->setCheckable(true);
    connect(_actionFullScreen, &QAction::triggered, this, &MainWindow::onFullscreen );  

    _actionLineNumbers = new QAction( "Line Numbers", this );
    _actionLineNumbers->setCheckable(true);
    connect(_actionLineNumbers, &QAction::triggered, _view, &MainView::showLineNumbers );   
    _actionFind = new QAction( QIcon::fromTheme("edit-find"), "Find", this );
    _actionFind->setShortcut(QKeySequence::Find);
    _actionFind->setCheckable(true);
    connect(_actionFind, &QAction::triggered, _view, &MainView::setSearchbarVisible );  
    _actionReplace = new QAction( QIcon::fromTheme("edit-replace"), "Replace", this );
    _actionReplace->setShortcut(QKeySequence::Replace);
    _actionReplace->setCheckable(true);
    connect(_actionReplace, &QAction::triggered, _view, &MainView::enableCurrentLineHighlighting );    
    
    _actionAboutQt = new QAction("About Qt", this );
    connect(_actionAboutQt,&QAction::triggered, qApp, &QApplication::aboutQt);  
    _actionAboutApp = new QAction("About", this );
    connect(_actionAboutApp, &QAction::triggered, this, &MainWindow::about);  
    
    _actionCut->setEnabled(false);
    _actionCopy->setEnabled(false);
    _actionUndo->setEnabled(false);
    _actionRedo->setEnabled(false);    
    connect(_view->textEdit(), &QPlainTextEdit::copyAvailable, _actionCut, &QAction::setEnabled);
    connect(_view->textEdit(), &QPlainTextEdit::copyAvailable, _actionCopy, &QAction::setEnabled);
    connect(_view->textEdit(), &QPlainTextEdit::undoAvailable, _actionUndo, &QAction::setEnabled);
    connect(_view->textEdit(), &QPlainTextEdit::redoAvailable, _actionRedo, &QAction::setEnabled);  
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
    _mainToolbar = addToolBar("Main Toolbar");
    
    _mainToolbar->addAction(_actionNew);
    _mainToolbar->addAction(_actionOpen);
    _mainToolbar->addAction(_actionSave);
    _mainToolbar->addSeparator();
    _mainToolbar->addAction(_actionPrint);
    _mainToolbar->addSeparator();
    _mainToolbar->addAction(_actionUndo);
    _mainToolbar->addAction(_actionRedo);
    _mainToolbar->addSeparator();
    _mainToolbar->addAction(_actionCut);
    _mainToolbar->addAction(_actionCopy);
    _mainToolbar->addAction(_actionPaste);

    // toolbar style and (position) lock
    _mainToolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    _mainToolbar->setMovable(false);
}


void MainWindow::setCurrentFilePath(const QString& path)
{
    QString curFile;
    if (path.isEmpty())
    {
        curFile = "untitled";
        _filePath = "";
    }
    else
    {
        curFile = QFileInfo(path).canonicalFilePath();
        _filePath = path;
    }

    _view->textEdit()->document()->setModified(false);
    setWindowModified(false);

    setWindowFilePath(curFile);
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
        setCurrentFilePath("");
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
        _mainToolbar->addAction(_actionFullScreen);
        _actionFullScreen->setText("Exit FullScreen");
        menuBar()->hide();
    }
    else
    {
        showNormal();
        _mainToolbar->removeAction(_actionFullScreen);
        _actionFullScreen->setText("FullScreen");
        menuBar()->show();
    }
}


void MainWindow::about()
{
    QString version = qApp->applicationVersion();
    
    QMessageBox::about(this, 
                       "About cutepad",
                        "cutepad " + version + "\n\nThe Qt pad ;)\nJust an easy plain text editor, based on Qt libraries");
}
