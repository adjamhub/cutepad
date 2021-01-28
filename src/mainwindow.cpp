/*
 * Copyright (C) Andrea Diamantini 2020-2021 <adjam@protonmail.com>
 *
 * CutePad project
 *
 * @license GPL-3.0 <https://www.gnu.org/licenses/gpl-3.0.txt>
 */


#include "mainwindow.h"
#include "application.h"
#include "settingsdialog.h"

#include <QCloseEvent>
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QScreen>
#include <QSettings>
#include <QStandardPaths>
#include <QStatusBar>
#include <QTextCodec>
#include <QTextStream>
#include <QToolBar>
#include <QVBoxLayout>

#include <QDebug>

#include <QPrinter>
#include <QPrintDialog>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , _textEdit(new TextEdit(this))
    , _searchBar(new SearchBar(this))
    , _replaceBar(new ReplaceBar(this))
    , _statusBar(new StatusBar(this))
    , _filePath("")
    , _zoomRange(0)
{
    setAttribute(Qt::WA_DeleteOnClose);

    // The UI
    QWidget* w = new QWidget(this);
    auto layout = new QVBoxLayout;
    layout->setContentsMargins (0, 0, 0, 0);
    layout->addWidget (_textEdit);
    layout->addWidget (_searchBar);
    layout->addWidget (_replaceBar);
    w->setLayout (layout);
    setCentralWidget(w);

    // let's start with the hidden bar(s)
    _searchBar->setVisible(false);
    _replaceBar->setVisible(false);

    connect(_searchBar, &SearchBar::search, this, &MainWindow::search);
    connect(this, &MainWindow::searchMessage, _searchBar, &SearchBar::searchMessage);

    connect(_replaceBar, &ReplaceBar::replace, this, &MainWindow::replace);

    // restore geometry and state
    QSettings s;
    restoreGeometry( s.value("geometry").toByteArray() );
    restoreState( s.value("myWidget/windowState").toByteArray() );

    // we need to load settings BEFORE setup actions,
    // to SET initial states
    loadSettings();

    setupActions();

    // application icon and title
    QIcon appIcon = QIcon::fromTheme( "accessories-text-editor" );
    setWindowIcon(appIcon);

    connect(_textEdit->document(), &QTextDocument::modificationChanged, this, &MainWindow::setWindowModified);
    setCurrentFilePath("");

    // take care of the statusbar
    statusBar()->addWidget(_statusBar);
    connect(_textEdit, &QPlainTextEdit::cursorPositionChanged, this, &MainWindow::updateStatusBar);

    updateStatusBar();
}


void MainWindow::loadSettings()
{
    // the settings object
    QSettings s;

    // options
    bool highlight = s.value("CurrentLineHighlight", false).toBool();
    _textEdit->enableCurrentLineHighlighting(highlight);

    QColor highlightLineColor = s.value("HighlightLineColor", QColor(Qt::yellow).lighter(160)).value<QColor>();
    _textEdit->setHighlightLineColor(highlightLineColor);

    int lineNumbers = s.value("LineNumbers", 0).toInt();
    _textEdit->setLineNumbersMode(lineNumbers);

    bool tabReplace = s.value("TabReplace", false).toBool();
    _textEdit->enableTabReplacement(tabReplace);
    
    int tabsCount = s.value("TabsCount", 4).toInt();
    _textEdit->setTabsCount(tabsCount);

    // font
    QString fontFamily = s.value("fontFamily", "Monospace").toString();
    int fontSize = s.value("fontSize", 12).toInt();
    int fontWeight = s.value("fontWeight", 50).toInt();
    bool italic = s.value("fontItalic", false).toBool();
    QFont font(fontFamily,fontSize + _zoomRange, fontWeight);
    font.setItalic(italic);
    _textEdit->setFont(font);
    QFontMetrics fm(font);
    _textEdit->setTabStopDistance( fm.horizontalAdvance( QChar(QChar::Space) ) * tabsCount );

    // syntax highlight theme
    QString sht = s.value("SyntaxHightlightTheme", "Breeze Light").toString();
    _textEdit->setSyntaxTheme(sht);
}


void MainWindow::tile(const QMainWindow *previous)
{
    if (!previous)
        return;

    // TODO: investigate about better positioning
    int topFrameWidth = previous->geometry().top() - previous->pos().y();
    if (!topFrameWidth) {
        topFrameWidth = 40;
    }

    const QPoint pos = previous->pos() + 2 * QPoint(topFrameWidth, topFrameWidth);
    if (screen()->availableGeometry().contains(rect().bottomRight() + pos)) {
        move(pos);
    }
}


void MainWindow::loadFilePath(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "Cannot open file. Not readable");
        return;
    }

    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

    QTextStream in(&file);
    QTextCodec* cod = in.codec();
    _textEdit->setTextCodec(cod);
    QString fileText = in.readAll();
    _textEdit->setPlainText(fileText);
    _textEdit->syntaxHighlightForFile(path);
    _textEdit->updateLineNumbersMode();
    setCurrentFilePath(path);

    QGuiApplication::restoreOverrideCursor();

    _textEdit->checkTabSpaceReplacementNeeded();
    updateStatusBar();
}


void MainWindow::saveFilePath(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "Cannot save file. Not writable");
        return;
    }

    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

    QString content = _textEdit->toPlainText();
    QTextCodec* codec = _textEdit->textCodec();
    QByteArray encodedString = codec->fromUnicode(content);
    
    QTextStream out(&file);
    out << encodedString;
    file.close();

    _textEdit->syntaxHighlightForFile(path);
    _textEdit->updateLineNumbersMode();

    QGuiApplication::restoreOverrideCursor();

    setCurrentFilePath(path);
    updateStatusBar();
}


bool MainWindow::exitAfterSaving()
{
    if (isWindowModified()) {

        int risp = QMessageBox::question(this,
                                         "Save Changes",
                                         "The file has unsaved changes",
                                         QMessageBox::Save | QMessageBox::No | QMessageBox::Cancel);

        switch(risp) {

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
    if (exitAfterSaving()) {
        QSettings s;
        s.setValue("geometry", saveGeometry());
        s.setValue("windowState", saveState());

        event->accept();
        return;
    }
    event->ignore();
}


void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {

        if (_replaceBar->isVisible()) {
            _searchBar->hide();
            _replaceBar->hide();
            event->accept();
            return;
        }

        if (_searchBar->isVisible()) {
            _searchBar->hide();
            event->accept();
            return;
        }

    }

    QMainWindow::keyPressEvent(event);
    return;
}


void MainWindow::setupActions()
{
    // ------------------------------------------------------------------------------------------------------------------------
    // Create and set ALL the needed actions

    // file actions -----------------------------------------------------------------------------------------------------------
    // NEW
    QAction* actionNew = new QAction( QIcon::fromTheme("document-new") , "New", this);
    actionNew->setShortcut(QKeySequence::New);
    connect(actionNew, &QAction::triggered, this, &MainWindow::newWindow);

    // OPEN
    QAction* actionOpen = new QAction( QIcon::fromTheme("document-open"), "Open", this);
    actionOpen->setShortcut(QKeySequence::Open);
    connect(actionOpen, &QAction::triggered, this, &MainWindow::openFile);

    // SAVE
    QAction* actionSave = new QAction( QIcon::fromTheme("document-save"), "Save", this);
    actionSave->setShortcut(QKeySequence::Save);
    connect(actionSave, &QAction::triggered, this, &MainWindow::saveFile);
    actionSave->setEnabled(false);
    connect(_textEdit->document(), &QTextDocument::modificationChanged, actionSave, &QAction::setEnabled);

    // SAVE AS
    QAction* actionSaveAs = new QAction( QIcon::fromTheme("document-save-as"), "Save As", this);
    connect(actionSaveAs, &QAction::triggered, this, &MainWindow::saveFileAs);

    // PRINT
    QAction* actionPrint = new QAction( QIcon::fromTheme("document-print"), "Print", this);
    actionPrint->setShortcut(QKeySequence::Print);
    connect(actionPrint, &QAction::triggered, this, &MainWindow::printFile);

    // CLOSE
    QAction* actionClose = new QAction( QIcon::fromTheme("document-close"), "Close", this);
    actionClose->setShortcut(QKeySequence::Close);
    connect(actionClose, &QAction::triggered, this, &MainWindow::close);

    // QUIT
    QAction* actionQuit = new QAction( QIcon::fromTheme("application-exit"), "Exit", this );
    actionQuit->setShortcut(QKeySequence::Quit);
    connect(actionQuit, &QAction::triggered, qApp, &QApplication::quit, Qt::QueuedConnection);

    // edit actions -----------------------------------------------------------------------------------------------------------
    // UNDO
    QAction* actionUndo = new QAction( QIcon::fromTheme("edit-undo"), "Undo", this );
    actionUndo->setShortcut(QKeySequence::Undo);
    connect(actionUndo, &QAction::triggered, _textEdit, &TextEdit::undo );
    actionUndo->setEnabled(false);
    connect(_textEdit, &QPlainTextEdit::undoAvailable, actionUndo, &QAction::setEnabled);

    // REDO
    QAction* actionRedo = new QAction(QIcon::fromTheme("edit-redo") , "Redo", this);
    actionRedo->setShortcut(QKeySequence::Redo);
    connect(actionRedo, &QAction::triggered, _textEdit, &TextEdit::redo );
    actionRedo->setEnabled(false);
    connect(_textEdit, &QPlainTextEdit::redoAvailable, actionRedo, &QAction::setEnabled);

    // CUT
    QAction* actionCut = new QAction(QIcon::fromTheme("edit-cut"), "Cut", this );
    actionCut->setShortcut(QKeySequence::Cut);
    connect(actionCut, &QAction::triggered, _textEdit, &TextEdit::cut );
    actionCut->setEnabled(false);
    connect(_textEdit, &QPlainTextEdit::copyAvailable, actionCut, &QAction::setEnabled);

    // COPY
    QAction* actionCopy = new QAction(QIcon::fromTheme("edit-copy"), "Copy", this );
    actionCopy->setShortcut(QKeySequence::Copy);
    connect(actionCopy, &QAction::triggered, _textEdit, &TextEdit::copy );
    actionCopy->setEnabled(false);
    connect(_textEdit, &QPlainTextEdit::copyAvailable, actionCopy, &QAction::setEnabled);

    // PASTE
    QAction* actionPaste = new QAction(QIcon::fromTheme("edit-paste"), "Paste", this );
    actionPaste->setShortcut(QKeySequence::Paste);
    connect(actionPaste, &QAction::triggered,  _textEdit, &TextEdit::paste );

    //SELECT ALL
    QAction* actionSelectAll = new QAction(QIcon::fromTheme("edit-select-all"), "Select All", this );
    actionSelectAll->setShortcut(QKeySequence::SelectAll);
    connect(actionSelectAll, &QAction::triggered, _textEdit, &TextEdit::selectAll );

    // view actions -----------------------------------------------------------------------------------------------------------
    // ZOOM IN
    QAction* actionZoomIn = new QAction( QIcon::fromTheme("zoom-in"), "Zoom In", this );
    actionZoomIn->setShortcut(QKeySequence::ZoomIn);
    connect(actionZoomIn, &QAction::triggered, this, &MainWindow::onZoomIn );

    // ZOOM OUT
    QAction* actionZoomOut = new QAction( QIcon::fromTheme("zoom-out"), "Zoom Out", this );
    actionZoomOut->setShortcut(QKeySequence::ZoomOut);
    connect(actionZoomOut, &QAction::triggered, this, &MainWindow::onZoomOut );

    // ZOOM ORIGINAL
    QAction* actionZoomOriginal = new QAction( QIcon::fromTheme("zoom-original"), "Zoom Original", this );
    actionZoomOriginal->setShortcut(Qt::CTRL + Qt::Key_0);
    connect(actionZoomOriginal, &QAction::triggered, this, &MainWindow::onZoomOriginal );

    // FULL SCREEN
    QAction* actionFullScreen = new QAction( QIcon::fromTheme("view-fullscreen"), "FullScreen", this );
    actionFullScreen->setShortcuts(QKeySequence::FullScreen);
    actionFullScreen->setCheckable(true);
    connect(actionFullScreen, &QAction::triggered, this, &MainWindow::onFullscreen );

    // find actions -----------------------------------------------------------------------------------------------------------
    // FIND
    QAction* actionFind = new QAction( QIcon::fromTheme("edit-find"), "Find", this );
    actionFind->setShortcut(QKeySequence::Find);
    connect(actionFind, &QAction::triggered, this, &MainWindow::showSearchBar );

    // REPLACE
    QAction* actionReplace = new QAction( QIcon::fromTheme("edit-replace"), "Replace", this );
    actionReplace->setShortcut(QKeySequence::Replace);
    connect(actionReplace, &QAction::triggered, this, &MainWindow::showReplaceBar );

    // option actions -----------------------------------------------------------------------------------------------------------
    // ENCODINGS
    QMenu* encodingsMenu = new QMenu("Encodings... ", this);

    QMenu* unicodeMenu = new QMenu("UNICODE", this);
    QMenu* iso8859Menu = new QMenu("ISO-8859", this);
    QMenu* ibmMenu = new QMenu("IBM", this);
    QMenu* windowsMenu = new QMenu("Windows", this);
    QMenu* othersMenu = new QMenu("Others", this);
    
    const QList<int> mibs = QTextCodec::availableMibs();
    QStringList codecsName;
    for (int mib : mibs) {
        QTextCodec *codec = QTextCodec::codecForMib(mib);
        QString name = codec->name().toUpper();
        if (codecsName.contains(name)) {
            continue;
        }
        codecsName << name;
        QAction *action = new QAction(name, this);
        action->setData(QVariant(name));
        connect(action, &QAction::triggered, this, &MainWindow::encode);
        if (name.startsWith("UTF-")) {
            unicodeMenu->addAction(action);
        } else
        if (name.startsWith("ISO-8859")) {
            iso8859Menu->addAction(action);
        } else
        if (name.startsWith("IBM")) {
            ibmMenu->addAction(action);
        } else
        if (name.startsWith("WINDOWS")) {
            windowsMenu->addAction(action);
        } else {
            othersMenu->addAction(action);
        }
    }

    encodingsMenu->addMenu(unicodeMenu);
    encodingsMenu->addMenu(iso8859Menu);
    encodingsMenu->addMenu(ibmMenu);
    encodingsMenu->addMenu(windowsMenu);
    encodingsMenu->addMenu(othersMenu);
    
    // SETTINGS
    QAction* actionShowSettings = new QAction( QIcon::fromTheme("settings-configure"), "Settings", this);
    connect(actionShowSettings, &QAction::triggered, this, &MainWindow::showSettings);

    // about actions -----------------------------------------------------------------------------------------------------------
    // MANUAL
    QAction* actionShowManual = new QAction( QIcon::fromTheme("help"), "Help", this );
    connect(actionShowManual, &QAction::triggered, this, &MainWindow::showManual);
    
    // ABOUT Qt
    QAction* actionAboutQt = new QAction( QIcon::fromTheme("qtlogo"), "About Qt", this );
    connect(actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);

    // ABOUT
    QAction* actionAboutApp = new QAction( QIcon::fromTheme("help-about"), "About", this );
    connect(actionAboutApp, &QAction::triggered, this, &MainWindow::about);

    // ------------------------------------------------------------------------------------------------------------------------


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

    QMenu* searchMenu = menuBar()->addMenu("&Search");
    searchMenu->addAction(actionFind);
    searchMenu->addAction(actionReplace);

    QMenu* optionsMenu = menuBar()->addMenu("&Options");
    optionsMenu->addMenu(encodingsMenu);
    optionsMenu->addSeparator();
    optionsMenu->addAction(actionShowSettings);

    QMenu* helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction(actionShowManual);
    helpMenu->addAction(actionAboutQt);
    helpMenu->addAction(actionAboutApp);

    // ------------------------------------------------------------------------------------------------------------------------
    // Create and set the MAIN TOOLBAR

    QToolBar* mainToolbar = addToolBar("Main Toolbar");
    mainToolbar->setObjectName("Main Toolbar");

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
    if (path.isEmpty()) {
        curFile = "untitled";
        _filePath = "";
    } else {
        curFile = QFileInfo(path).canonicalFilePath();
        _filePath = path;
    }

    _textEdit->document()->setModified(false);
    setWindowModified(false);

    setWindowFilePath(curFile);
}


void MainWindow::newWindow()
{
    Application::instance()->loadPath("");
}


void MainWindow::openFile()
{
    QString path = QFileDialog::getOpenFileName(this);
    if (path.isEmpty())
        return;
    
    if (_filePath.isEmpty() && !isWindowModified()) {
        loadFilePath(path);
        return;
    }

    MainWindow *other = new MainWindow;
    other->tile(this);
    other->show();
    other->loadFilePath(path);
}


void MainWindow::saveFile()
{
    if (_filePath.isEmpty()) {
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
    if (info.fileName() == info.baseName()) {
        path += ".txt";
    }

    saveFilePath(path);
}


void MainWindow::printFile()
{
    QPrinter printer;
    QPrintDialog printDialog(&printer, this);
    if (printDialog.exec() == QDialog::Accepted) {
        _textEdit->print(&printer);
    }
}


void MainWindow::onZoomIn()
{
    _zoomRange++;
    _textEdit->zoomIn();
    updateStatusBar();
}


void MainWindow::onZoomOut()
{
    _zoomRange--;
    _textEdit->zoomOut();
    updateStatusBar();
}


void MainWindow::onZoomOriginal()
{
    if (_zoomRange > 0) {
        _textEdit->zoomOut( _zoomRange );
    } else {
        _textEdit->zoomIn( _zoomRange * -1 );
    }
    _zoomRange = 0;
    updateStatusBar();
}


void MainWindow::onFullscreen(bool on)
{
    if (on) {
        showFullScreen();
        menuBar()->hide();
    } else {
        showNormal();
        menuBar()->show();
    }
}


void MainWindow::about()
{
    QString version = qApp->applicationVersion();

    QString aboutText = "";
    aboutText += "<h1>Cutepad " + version + "</h1>";
    aboutText += "<p>";
    aboutText += "The Qt pad ;)<br>Just an easy plain text editor, based on Qt libraries";
    aboutText += "</p><p>";
    aboutText += "(c) 2020-2021 <a href='mailto:adjam@protonmail.com'>Andrea Diamantini</a> (adjam)";
    aboutText += "</p>";
    aboutText += "<a href='https://github.com/adjamhub/cutepad'>https://github.com/adjamhub/cutepad</a>";
    aboutText += "<br>";

    QMessageBox::about(this, "About cutepad", aboutText);
}


void MainWindow::showManual()
{
    QStringList dirs = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);
    qDebug() << "dirs: " << dirs;
    QString manual = QStandardPaths::locate(QStandardPaths::AppDataLocation, "MANUAL");
    qDebug() << "manual path: " << manual;
    if (manual.isEmpty())
        return;
    // TODO: needs to advise on return
    
    MainWindow *other = new MainWindow;
    other->tile(this);
    other->show();
    other->loadFilePath(manual);
    // FIXME other->view()->textEdit()->setReadOnly(true);
}


void MainWindow::updateStatusBar()
{
    _statusBar->setLanguage(_textEdit->language());

    int row = _textEdit->textCursor().blockNumber();
    int col = _textEdit->textCursor().positionInBlock();
    _statusBar->setPosition(row,col);

    QTextCodec* cod = _textEdit->textCodec();
    QString codecText = "none";
    if (cod) {
        codecText = cod->name();
    }
    _statusBar->setCodec(codecText);

    QString zoomText = "100%";
    if (_zoomRange != 0) {
        int z = 100 + _zoomRange * 10;
        zoomText = QString::number(z) + "%";
    }
    _statusBar->setZoom(zoomText);
}


void MainWindow::encode()
{
    const QAction *action = qobject_cast<const QAction *>(sender());
    const QByteArray codecName = action->data().toByteArray();
    QTextCodec* targetCodec = QTextCodec::codecForName(codecName);
    QTextCodec* actualCodec = _textEdit->textCodec();

    if (codecName == actualCodec->name()) {
        qDebug() << "we are moving to the same codec. Aborting...";
        return;
    }
    QString content = _textEdit->toPlainText();

    // FIXME: What about "state"???
    QByteArray encodedData = actualCodec->fromUnicode(content);
    QTextCodec::ConverterState state;
    QString decodedString = targetCodec->toUnicode(encodedData.constData(), encodedData.size(), &state);
    
    _textEdit->setPlainText(decodedString);
    _textEdit->setTextCodec(targetCodec);

    _textEdit->document()->setModified(true);
    setWindowModified(true);

    updateStatusBar();
}


void MainWindow::showSettings()
{
    SettingsDialog* dialog = new SettingsDialog(this);
    dialog->exec();
    dialog->deleteLater();

    Application::instance()->loadSettings();
}


void MainWindow::showSearchBar()
{
    if (_replaceBar->isVisible()) {
        _replaceBar->hide();
        return;
    }

    if (_searchBar->isVisible()) {
        _searchBar->hide();
        return;
    }

    _searchBar->show();

    // if text is selected, copy to lineEdit
    QString sel = _textEdit->textCursor().selectedText();
    if (!sel.isEmpty()) {
        _searchBar->_findLineEdit->setText(sel);
    }

    _searchBar->setFocus();
}


void MainWindow::showReplaceBar()
{
    if (_replaceBar->isVisible()) {
        _searchBar->hide();
        _replaceBar->hide();
        return;
    }

    _searchBar->show();
    _replaceBar->show();

    // if text is selected, copy to FIND lineEdit
    QString sel = _textEdit->textCursor().selectedText();
    if (!sel.isEmpty()) {
        _searchBar->_findLineEdit->setText(sel);
    }

    _replaceBar->setFocus();
}


void MainWindow::search(const QString & search, bool forward, bool casesensitive)
{
    QTextDocument::FindFlags flags;

    if (!forward) {
        flags |= QTextDocument::FindBackward;
    }

    if (casesensitive) {
        flags |= QTextDocument::FindCaseSensitively;
    }

    bool found = _textEdit->find(search, flags);
    if (!found) {
        QTextCursor cur = _textEdit->textCursor();
        if (forward) {
            cur.movePosition(QTextCursor::Start);
        } else {
            cur.movePosition(QTextCursor::End);
        }
        _textEdit->setTextCursor(cur);
        emit searchMessage("Search restarted");
        found = _textEdit->find(search, flags);
        if (!found) {
            emit searchMessage("not found");
            return;
        }
    }
}


void MainWindow::replace(const QString &replace, bool justNext)
{
    QString search = _searchBar->_findLineEdit->text();
    bool matchCase = _searchBar->_caseCheckBox->isChecked();

    if (search.isEmpty()) {
        return;
    }

    Qt::CaseSensitivity cs = matchCase ? Qt::CaseSensitive : Qt::CaseInsensitive;
    QString content = _textEdit->toPlainText();

    if (!justNext) {
        content.replace(search, replace, cs);
        _textEdit->setPlainText(content);
        _textEdit->document()->setModified(true);
        return;
    }

    QTextDocument::FindFlags flags;

    if (matchCase) {
        flags |= QTextDocument::FindCaseSensitively;
    }

    bool found = _textEdit->find(search, flags);
    if (!found) {
        QTextCursor cur = _textEdit->textCursor();
        cur.movePosition(QTextCursor::Start);
        _textEdit->setTextCursor(cur);
        emit searchMessage("Search restarted");
        found = _textEdit->find(search, flags);
        if (!found) {
            emit searchMessage("not found");
            return;
        }
    }
    int n = search.length();
    int position = _textEdit->textCursor().position() - n;
    content.replace(position, n, replace);
    _textEdit->setPlainText(content);
    _textEdit->document()->setModified(true);
    QTextCursor cur = _textEdit->textCursor();
    cur.setPosition(position + n);
    _textEdit->setTextCursor(cur);
    return;
}
