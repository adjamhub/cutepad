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
#include <QFontDialog>
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
    , _zoomRange(0)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setCentralWidget(_view);

	// we need to load settings BEFORE setup actions,
	// to SET initial states
    loadSettings();
    
    setupActions();
     
    // application icon and title
    QIcon appIcon = QIcon::fromTheme( "accessories-text-editor" );
    setWindowIcon(appIcon);
    
    connect(_view->textEdit()->document(), &QTextDocument::modificationChanged, this, &MainWindow::setWindowModified);
    setCurrentFilePath("");
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
    
    // textEdit
    bool highlight = s.value("CurrentLineHighlight", false).toBool();
    _view->textEdit()->enableCurrentLineHighlighting(highlight);
    bool lineNumbers = s.value("LineNumbers", false).toBool();
    _view->textEdit()->enableLineNumbers(lineNumbers);
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
    int fontSize = f.pointSize() - _zoomRange; // consider the zoom...
    int fontWeight = f.weight();
    bool italic = f.italic();
    s.setValue("fontFamily", fontFamily);
    s.setValue("fontSize", fontSize);
    s.setValue("fontWeight", fontWeight);
    s.setValue("fontItalic", italic);
    
    // textEdit
    bool highlight = _view->textEdit()->isCurrentLineHighlightingEnabled();
	s.setValue("CurrentLineHighlight", highlight);
    bool lineNumbers = _view->textEdit()->isLineNumbersEnabled();
    s.setValue("LineNumbers", lineNumbers);
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
	// ------------------------------------------------------------------------------------------------------------------------
	// Create and set ALL the needed actions
	
    // file actions -----------------------------------------------------------------------------------------------------------
    QAction* actionNew = new QAction( QIcon::fromTheme("document-new") , "New", this);
    actionNew->setShortcut(QKeySequence::New);
    connect(actionNew, &QAction::triggered, this, &MainWindow::newWindow);      
    
    QAction* actionOpen = new QAction( QIcon::fromTheme("document-open"), "Open", this);
    actionOpen->setShortcut(QKeySequence::Open);
    connect(actionOpen, &QAction::triggered, this, &MainWindow::openFile);  
    
    QAction* actionSave = new QAction( QIcon::fromTheme("document-save"), "Save", this);
    actionSave->setShortcut(QKeySequence::Save);
    connect(actionSave, &QAction::triggered, this, &MainWindow::saveFile);
    actionSave->setEnabled(false);
    connect(_view->textEdit()->document(), &QTextDocument::modificationChanged, actionSave, &QAction::setEnabled);
    
    QAction* actionSaveAs = new QAction( QIcon::fromTheme("document-save-as"), "Save As", this);
    connect(actionSaveAs, &QAction::triggered, this, &MainWindow::saveFileAs);  
    
    QAction* actionPrint = new QAction( QIcon::fromTheme("document-print"), "Print", this);
    actionPrint->setShortcut(QKeySequence::Print);
    connect(actionPrint, &QAction::triggered, this, &MainWindow::printFile);    
    
    QAction* actionClose = new QAction( QIcon::fromTheme("document-close"), "Close", this);
    actionClose->setShortcut(QKeySequence::Close);
    connect(actionClose, &QAction::triggered, this, &MainWindow::closeFile);    
    
    QAction* actionQuit = new QAction( QIcon::fromTheme("application-exit"), "Exit", this );
    actionQuit->setShortcut(QKeySequence::Quit);
    // qApp->quit() does NOT save settings!!! Needs investigation
    connect(actionQuit, &QAction::triggered, this, &MainWindow::close);
    
    // edit actions -----------------------------------------------------------------------------------------------------------
    QAction* actionUndo = new QAction( QIcon::fromTheme("edit-undo"), "Undo", this );
    actionUndo->setShortcut(QKeySequence::Undo);
    connect(actionUndo, &QAction::triggered, _view->textEdit(), &TextEdit::undo );  
    
    QAction* actionRedo = new QAction(QIcon::fromTheme("edit-redo") , "Redo", this);
    actionRedo->setShortcut(QKeySequence::Redo);
    connect(actionRedo, &QAction::triggered, _view->textEdit(), &TextEdit::redo );  
    
    QAction* actionCut = new QAction(QIcon::fromTheme("edit-cut"), "Cut", this );
    actionCut->setShortcut(QKeySequence::Cut);
    connect(actionCut, &QAction::triggered, _view->textEdit(), &TextEdit::cut );    
    
    QAction* actionCopy = new QAction(QIcon::fromTheme("edit-copy"), "Copy", this );
    actionCopy->setShortcut(QKeySequence::Copy);
    connect(actionCopy, &QAction::triggered, _view->textEdit(), &TextEdit::copy );  
    
    QAction* actionPaste = new QAction(QIcon::fromTheme("edit-paste"), "Paste", this );
    actionPaste->setShortcut(QKeySequence::Paste);
    connect(actionPaste, &QAction::triggered,  _view->textEdit(), &TextEdit::paste );   
    
    QAction* actionSelectAll = new QAction(QIcon::fromTheme("edit-select-all"), "Select All", this );
    actionSelectAll->setShortcut(QKeySequence::SelectAll);
    connect(actionSelectAll, &QAction::triggered, _view->textEdit(), &TextEdit::selectAll );
    
    // view actions -----------------------------------------------------------------------------------------------------------
    QAction* actionZoomIn = new QAction( QIcon::fromTheme("zoom-in"), "Zoom In", this );
    actionZoomIn->setShortcut(QKeySequence::ZoomIn);
    connect(actionZoomIn, &QAction::triggered, this, &MainWindow::onZoomIn );  
    
    QAction* actionZoomOut = new QAction( QIcon::fromTheme("zoom-out"), "Zoom Out", this );
    actionZoomOut->setShortcut(QKeySequence::ZoomOut);
    connect(actionZoomOut, &QAction::triggered, this, &MainWindow::onZoomOut ); 
    
    QAction* actionZoomOriginal = new QAction( QIcon::fromTheme("zoom-original"), "Zoom Original", this );
    actionZoomOriginal->setShortcut(Qt::CTRL + Qt::Key_0);
    connect(actionZoomOriginal, &QAction::triggered, this, &MainWindow::onZoomOriginal );    

    QAction* actionFullScreen = new QAction( QIcon::fromTheme("view-fullscreen"), "FullScreen", this );
    actionFullScreen->setShortcuts(QKeySequence::FullScreen);
    actionFullScreen->setCheckable(true);
    connect(actionFullScreen, &QAction::triggered, this, &MainWindow::onFullscreen );  

    // xxx actions -----------------------------------------------------------------------------------------------------------
    QAction* actionLineNumbers = new QAction( "Line Numbers", this );
    actionLineNumbers->setCheckable(true);
    actionLineNumbers->setChecked(_view->textEdit()->isLineNumbersEnabled());
    connect(actionLineNumbers, &QAction::triggered, _view->textEdit(), &TextEdit::enableLineNumbers );   
    
    QAction* actionCurrentLineHighlight = new QAction( "Current Line Highlight", this );
    actionCurrentLineHighlight->setCheckable(true);
    actionCurrentLineHighlight->setChecked(_view->textEdit()->isCurrentLineHighlightingEnabled());
    connect(actionCurrentLineHighlight, &QAction::triggered, _view->textEdit(), &TextEdit::enableCurrentLineHighlighting );   
    
    QAction* actionFind = new QAction( QIcon::fromTheme("edit-find"), "Find", this );
    actionFind->setShortcut(QKeySequence::Find);
    connect(actionFind, &QAction::triggered, _view, &MainView::showSearchbar );  
    
    QAction* actionReplace = new QAction( QIcon::fromTheme("edit-replace"), "Replace", this );
    actionReplace->setShortcut(QKeySequence::Replace);
    connect(actionReplace, &QAction::triggered, _view, &MainView::showReplaceBar );    

    QAction* actionFontChange = new QAction( QIcon::fromTheme("applications-fonts"), "Font", this );
    connect(actionFontChange, &QAction::triggered, this, &MainWindow::selectFont );    
    
    // about actions -----------------------------------------------------------------------------------------------------------
    QAction* actionAboutQt = new QAction("About Qt", this );
    connect(actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);  
    
    QAction* actionAboutApp = new QAction("About", this );
    connect(actionAboutApp, &QAction::triggered, this, &MainWindow::about);  
    
    actionCut->setEnabled(false);
    actionCopy->setEnabled(false);
    actionUndo->setEnabled(false);
    actionRedo->setEnabled(false);    
    connect(_view->textEdit(), &QPlainTextEdit::copyAvailable, actionCut, &QAction::setEnabled);
    connect(_view->textEdit(), &QPlainTextEdit::copyAvailable, actionCopy, &QAction::setEnabled);
    connect(_view->textEdit(), &QPlainTextEdit::undoAvailable, actionUndo, &QAction::setEnabled);
    connect(_view->textEdit(), &QPlainTextEdit::redoAvailable, actionRedo, &QAction::setEnabled);
    
    // ------------------------------------------------------------------------------------------------------------------------
	// Create and set the MENUBAR

    QMenu* fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction(actionNew);
    fileMenu->addAction(actionOpen);
    fileMenu->addAction(actionSave);   
    fileMenu->addAction(actionSaveAs); 
    fileMenu->addSeparator();
    fileMenu->addAction(actionPrint);  
    fileMenu->addSeparator();
    fileMenu->addAction(actionClose);  
    fileMenu->addAction(actionQuit);   

    QMenu* editMenu = menuBar()->addMenu("&Edit");
    editMenu->addAction(actionUndo);
    editMenu->addAction(actionRedo);
    editMenu->addSeparator();
    editMenu->addAction(actionCut);
    editMenu->addAction(actionCopy);
    editMenu->addAction(actionPaste);
    editMenu->addSeparator();
    editMenu->addAction(actionSelectAll);
    
    QMenu* viewMenu = menuBar()->addMenu("&View");
    viewMenu->addAction(actionZoomIn);
    viewMenu->addAction(actionZoomOut);
    viewMenu->addAction(actionZoomOriginal);
    viewMenu->addSeparator();
    viewMenu->addAction(actionFullScreen);

    QMenu* optionsMenu = menuBar()->addMenu("&Options");
    optionsMenu->addAction(actionLineNumbers);
    optionsMenu->addAction(actionCurrentLineHighlight);
    optionsMenu->addSeparator();
    optionsMenu->addAction(actionFind);
    optionsMenu->addAction(actionReplace);
    optionsMenu->addSeparator();
    optionsMenu->addAction(actionFontChange);
    
    QMenu* helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction(actionAboutQt);
    helpMenu->addAction(actionAboutApp);   
    
    // ------------------------------------------------------------------------------------------------------------------------
	// Create and set the MAIN TOOLBAR

    QToolBar* mainToolbar = addToolBar("Main Toolbar");
    
    mainToolbar->addAction(actionNew);
    mainToolbar->addAction(actionOpen);
    mainToolbar->addAction(actionSave);
    mainToolbar->addSeparator();
    mainToolbar->addAction(actionPrint);
    mainToolbar->addSeparator();
    mainToolbar->addAction(actionUndo);
    mainToolbar->addAction(actionRedo);
    mainToolbar->addSeparator();

    connect(actionFullScreen, &QAction::triggered, this, [=](bool on) { 
    		if (on) { 
    			mainToolbar->addAction(actionFullScreen); 
    			actionFullScreen->setText("Exit FullScreen");
    		} else { 
    			mainToolbar->removeAction(actionFullScreen); 
    			actionFullScreen->setText("FullScreen");
    		}
    	}
    );
    
    // toolbar style and (position) lock
    mainToolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    mainToolbar->setMovable(false);
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
    _zoomRange++;
	_view->textEdit()->zoomIn();
}


void MainWindow::onZoomOut()
{
    _zoomRange--;
    _view->textEdit()->zoomOut();
}


void MainWindow::onZoomOriginal()
{
	if (_zoomRange > 0) {
		_view->textEdit()->zoomOut( _zoomRange );
	} else {
		_view->textEdit()->zoomIn( _zoomRange * -1 );
	}
	_zoomRange = 0;
}


void MainWindow::onFullscreen(bool on)
{
    if (on)
    {
        showFullScreen();
        menuBar()->hide();
    }
    else
    {
        showNormal();
        menuBar()->show();
    }
}


void MainWindow::selectFont()
{
    bool ok;
    // initialFont is the font initially shown in the dialog
    QFont initialFont = _view->textEdit()->font();
    initialFont.setPointSize( initialFont.pointSize() - _zoomRange );
    QFont font = QFontDialog::getFont(&ok, initialFont);
    if (!ok)
        return;

    _view->textEdit()->setFont(font);
    _view->textEdit()->setFocus();
}


void MainWindow::about()
{
    QString version = qApp->applicationVersion();
    
    QMessageBox::about(this, 
                       "About cutepad",
                        "cutepad " + version + "\n\nThe Qt pad ;)\nJust an easy plain text editor, based on Qt libraries");
}
