/***************************************************************************
 *   Copyright (C) 2012 by santiago González                               *
 *   santigoro@gmail.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, see <http://www.gnu.org/licenses/>.  *
 *                                                                         *
 ***************************************************************************/

//#include <QtGui>

#include "editorwindow.h"
#include "utils.h"

EditorWindow::EditorWindow( QWidget* parent )
            : QWidget( parent )
{
    createWidgets();
    createActions();
    createToolBars();
    readSettings();    
    //newFile();

    // Redirect stdout to outPane
    /*mybuf = new mystreambuf( outPane );
    std::cout.rdbuf( mybuf );*/
}
EditorWindow::~EditorWindow(){ writeSettings(); }

void EditorWindow::closeEvent(QCloseEvent* event)
{
    Q_UNUSED( event );
    maybeSave();
    writeSettings();
}

void EditorWindow::newFile()
{
    CodeEditorWidget* baseWidget = new CodeEditorWidget( this );
    docWidget->addTab( baseWidget, "New" );
    docWidget->setCurrentWidget( baseWidget );
    connect(baseWidget->m_codeEditor->document(), SIGNAL(contentsChanged()),
            this,                                 SLOT(documentWasModified()));
    m_fileList << "New";
}

void EditorWindow::open()
{
    const QString dir = m_lastDir;
    QString fileName = QFileDialog::getOpenFileName( this, tr("Load File"), dir, tr("All files (*)"));
    if (!fileName.isEmpty()) loadFile(fileName);
}

void EditorWindow::loadFile(const QString &fileName)
{
    if( m_fileList.contains( fileName ) )
    {
        docWidget->setCurrentIndex( m_fileList.indexOf( fileName ) );
        return;
    }
    newFile();
    QApplication::setOverrideCursor(Qt::WaitCursor);
    CodeEditor* ce = getCodeEditor();
    ce->setPlainText( fileToString( fileName, "EditorWindow" ) );
    ce->setFile( fileName );
    m_lastDir = fileName;
    int index = docWidget->currentIndex();
    m_fileList.replace( index, fileName );
    docWidget->setTabText( index, strippedName(fileName) );
    enableFileActs( true );   // enable file actions
    //if( ce->hasDebugger() )
        enableDebugActs( true );
    QApplication::restoreOverrideCursor();
}

bool EditorWindow::save()
{
    QString file = getCodeEditor()->getFilePath();
    if( file.isEmpty() ) return saveAs();
    else                 return saveFile(file);
}

bool EditorWindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this);
    if (fileName.isEmpty()) return false;

    m_fileList.replace( docWidget->currentIndex(), fileName );

    return saveFile(fileName);
}

bool EditorWindow::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }
    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    CodeEditor* ce = getCodeEditor();
    out << ce->toPlainText();
    //ce->setFile( fileName );
    QApplication::restoreOverrideCursor();
    
    ce->document()->setModified(false);
    documentWasModified();

    docWidget->setTabText( docWidget->currentIndex(), strippedName(fileName) );
    return true;
}

bool EditorWindow::maybeSave()
{
    if( m_fileList.isEmpty() ) return false;
    if( getCodeEditor()->document()->isModified() )
    {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("Application"),
              tr("The document has been modified.\nDo you want to save your changes?"),
              QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if      (ret == QMessageBox::Save)   return save();
        else if (ret == QMessageBox::Cancel) return false;
    }
    return true;
}

void EditorWindow::documentWasModified(  )
{
    QTextDocument *doc     = getCodeEditor()->document();
    
    bool    modified = doc->isModified();
    int     index      = docWidget->currentIndex();
    QString tabText  = docWidget->tabText( index );

    if     ( modified && !tabText.endsWith("*") ) tabText.append("*");
    else if( !modified && tabText.endsWith("*") ) tabText.remove("*");

    docWidget->setTabText( index, tabText );

    redoAct->setEnabled( false );
    undoAct->setEnabled( false );
    if( doc->isRedoAvailable() ) redoAct->setEnabled( true );
    if( doc->isUndoAvailable() ) undoAct->setEnabled( true );
}

void EditorWindow::enableFileActs( bool enable )
{
    saveAct->setEnabled( enable );
    saveAsAct->setEnabled( enable );
    cutAct->setEnabled( enable );
    copyAct->setEnabled( enable );
    pasteAct->setEnabled( enable );
    undoAct->setEnabled( enable );
    redoAct->setEnabled( enable );
    findQtAct->setEnabled( enable );
}

void EditorWindow::enableDebugActs( bool enable )
{
    debugAct->setEnabled( enable );
    runAct->setEnabled( enable );
    stepAct->setEnabled( enable );
    stepOverAct->setEnabled( enable );
    pauseAct->setEnabled( enable );
    resetAct->setEnabled( enable );
    stopAct->setEnabled( enable );
    compileAct->setEnabled( enable );
    loadAct->setEnabled( enable );
}

void EditorWindow::tabContextMenu( const QPoint &eventpoint )
{
    CodeEditor* ce = getCodeEditor();
    if( !ce ) return;
    
    QMenu* menu = new QMenu();
    QAction* setCompilerAction = menu->addAction(QIcon(":/copy.png"),"set Compiler Path");
    connect( setCompilerAction, SIGNAL( triggered()), this, SLOT(setCompiler()) );

    menu->exec( mapToGlobal(eventpoint) );
    menu->deleteLater();
}

void EditorWindow::setCompiler()
{
    CodeEditor* ce = getCodeEditor();
    if( ce ) ce->setCompilerPath();
}

void EditorWindow::createWidgets()
{
    baseWidgetLayout = new QGridLayout( this );
    baseWidgetLayout->setSpacing(0);
    baseWidgetLayout->setContentsMargins(0, 0, 0, 0);
    baseWidgetLayout->setObjectName("gridLayout");
    
    m_editorToolBar = new QToolBar( this );
    baseWidgetLayout->addWidget( m_editorToolBar );
    m_debuggerToolBar = new QToolBar( this );
    m_debuggerToolBar->setVisible( false );
    baseWidgetLayout->addWidget( m_debuggerToolBar );
    

    docWidget = new QTabWidget( this );
    docWidget->setObjectName("docWidget");
    docWidget->setTabPosition( QTabWidget::North );
    docWidget->setTabsClosable ( true );
    docWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    //docWidget->setMovable( true );
    baseWidgetLayout->addWidget( docWidget );
    connect( docWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
    connect( docWidget, SIGNAL(customContextMenuRequested(const QPoint &)), SLOT(tabContextMenu(const QPoint &)));
    //connect( docWidget, SIGNAL(currentChanged(int)), this, SLOT(documentWasModified(int)));
    setLayout( baseWidgetLayout );
    
    findRepDiaWidget = new FindReplaceDialog(this);
    findRepDiaWidget->setModal(false);
}

void EditorWindow::createActions()
{
    newAct = new QAction(QIcon(":/new.png"), tr("&New"), this);
    newAct->setShortcut( QKeySequence(Qt::CTRL + Qt::Key_N) );
    newAct->setStatusTip(tr("Create a new file"));
    connect( newAct, SIGNAL(triggered()), this, SLOT(newFile()));

    openAct = new QAction(QIcon(":/open.png"), tr("&Open..."), this);
    openAct->setShortcut( QKeySequence(Qt::CTRL + Qt::Key_O) );
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    saveAct = new QAction(QIcon(":/save.png"), tr("&Save"), this);
    saveAct->setShortcut( QKeySequence(Qt::CTRL + Qt::Key_S) );
    //saveAct->setShortcutContext( Qt::WidgetShortcut );
    saveAct->setStatusTip(tr("Save the document to disk"));
    saveAct->setEnabled(false);
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAct = new QAction(QIcon(":/saveas.png"),tr("Save &As..."), this);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));
    saveAsAct->setEnabled(false);
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    exitAct = new QAction(QIcon(":/exit.png"),tr("E&xit"), this);
    exitAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    cutAct = new QAction(QIcon(":/cut.png"), tr("Cu&t"), this);
    cutAct->setShortcut( QKeySequence(Qt::CTRL + Qt::Key_X) );
    cutAct->setStatusTip(tr("Cut the current selection's contents to the clipboard"));
    cutAct->setEnabled(false);
    connect(cutAct, SIGNAL(triggered()), this, SLOT(cut()));

    copyAct = new QAction(QIcon(":/copy.png"), tr("&Copy"), this);
    copyAct->setShortcut( QKeySequence(Qt::CTRL + Qt::Key_C) );
    copyAct->setStatusTip(tr("Copy the current selection's contents to the clipboard"));
    copyAct->setEnabled(false);
    connect(copyAct, SIGNAL(triggered()), this, SLOT(copy()));

    pasteAct = new QAction(QIcon(":/paste.png"), tr("&Paste"), this);
    pasteAct->setShortcut( QKeySequence(Qt::CTRL + Qt::Key_V) );
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current selection"));
    pasteAct->setEnabled(false);
    connect(pasteAct, SIGNAL(triggered()), this, SLOT(paste()));

    undoAct = new QAction(QIcon(":/undo.png"), tr("Undo"), this);
    undoAct->setStatusTip(tr("Undo the last action"));
    undoAct->setEnabled(false);
    connect(undoAct, SIGNAL(triggered()), this, SLOT(undo()));

    redoAct = new QAction(QIcon(":/redo.png"), tr("Redo"), this);
    redoAct->setStatusTip(tr("Redo the last action"));
    redoAct->setEnabled(false);
    connect(redoAct, SIGNAL(triggered()), this, SLOT(redo()));

    runAct =  new QAction(QIcon(":/play.png"),tr("Run To Breakpoint"), this);
    runAct->setStatusTip(tr("Run to next breakpoint"));
    runAct->setEnabled(false);
    connect(runAct, SIGNAL(triggered()), this, SLOT(run()));

    stepAct = new QAction(QIcon(":/step.png"),tr("Step"), this);
    stepAct->setStatusTip(tr("Step debugger"));
    stepAct->setEnabled(false);
    connect( stepAct, SIGNAL(triggered()), this, SLOT(step()) );

    stepOverAct = new QAction(QIcon(":/stepover.png"),tr("StepOver"), this);
    stepOverAct->setStatusTip(tr("Step Over"));
    stepOverAct->setEnabled(false);
    connect( stepOverAct, SIGNAL(triggered()), this, SLOT(stepOver()) );

    pauseAct = new QAction(QIcon(":/pause.png"),tr("Pause"), this);
    pauseAct->setStatusTip(tr("Pause debugger"));
    pauseAct->setEnabled(false);
    connect( pauseAct, SIGNAL(triggered()), this, SLOT(pause()) );

    resetAct = new QAction(QIcon(":/reset.png"),tr("Reset"), this);
    resetAct->setStatusTip(tr("Reset debugger"));
    resetAct->setEnabled(false);
    connect( resetAct, SIGNAL(triggered()), this, SLOT(reset()) );

    stopAct = new QAction(QIcon(":/stop.png"),tr("Stop"), this);
    stopAct->setStatusTip(tr("Stop debugger"));
    stopAct->setEnabled(false);
    connect( stopAct, SIGNAL(triggered()), this, SLOT(stop()) );

    compileAct = new QAction(QIcon(":/compile.png"),tr("Compile"), this);
    compileAct->setStatusTip(tr("Compile Source"));
    compileAct->setEnabled(false);
    connect( compileAct, SIGNAL(triggered()), this, SLOT(compile()) );
    
    loadAct = new QAction(QIcon(":/load.png"),tr("UpLoad"), this);
    loadAct->setStatusTip(tr("Load Firmware"));
    loadAct->setEnabled(false);
    connect( loadAct, SIGNAL(triggered()), this, SLOT(upload()) );

    aboutAct = new QAction(QIcon(":/info.png"),tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(QIcon(":/info.png"),tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    //connect(m_codeEditor, SIGNAL(copyAvailable(bool)), cutAct, SLOT(setEnabled(bool)));
    //connect(m_codeEditor, SIGNAL(copyAvailable(bool)), copyAct, SLOT(setEnabled(bool)));
    
    findQtAct = new QAction(QIcon(":/find.png"),tr("Find Replace"), this);
    findQtAct->setStatusTip(tr("Find Replace"));
    findQtAct->setEnabled(false);
    connect(findQtAct, SIGNAL(triggered()), this, SLOT(findReplaceDialog()));
    
    debugAct =  new QAction(QIcon(":/play.png"),tr("Debug"), this);
    debugAct->setStatusTip(tr("Start Debugger"));
    debugAct->setEnabled(false);
    connect(debugAct, SIGNAL(triggered()), this, SLOT(debug()));
}

CodeEditor* EditorWindow::getCodeEditor()
{
    CodeEditorWidget* actW = dynamic_cast<CodeEditorWidget*>(docWidget->currentWidget());
    if( actW )return actW->m_codeEditor;
    else      return 0l;
}

void EditorWindow::closeTab( int index )
{
    maybeSave();
    m_fileList.removeAt(index);
    
    if( m_fileList.isEmpty() )
    {
        enableFileActs( false ); // disable file actions
        enableDebugActs( false );
    }
    
    CodeEditorWidget *actW = dynamic_cast<CodeEditorWidget*>(docWidget->widget(index));
    docWidget->removeTab( index );
    delete actW;

    int last = docWidget->count()-1;
    if( index > last ) docWidget->setCurrentIndex( last );
    else               docWidget->setCurrentIndex( index );
}

void EditorWindow::cut()     { getCodeEditor()->cut(); }
void EditorWindow::copy()    { getCodeEditor()->copy(); }
void EditorWindow::paste()   { getCodeEditor()->paste(); }
void EditorWindow::undo()    { getCodeEditor()->undo(); }
void EditorWindow::redo()    { getCodeEditor()->redo(); }

void EditorWindow::debug()    
{ 
    CodeEditor* ce = getCodeEditor();
    ce->step();
    if( ce->debugStarted() )
    {
        m_editorToolBar->setVisible( false);
        m_debuggerToolBar->setVisible( true );
    }
}
void EditorWindow::run()     { getCodeEditor()->run(); }
void EditorWindow::step()    { getCodeEditor()->step(); }
void EditorWindow::stepOver(){ getCodeEditor()->stepOver(); }
void EditorWindow::pause()   { getCodeEditor()->pause(); }
void EditorWindow::reset()   { getCodeEditor()->reset(); }
void EditorWindow::stop()    
{ 
    getCodeEditor()->stopDebbuger(); 
    m_debuggerToolBar->setVisible( false );
    m_editorToolBar->setVisible( true);
}
void EditorWindow::compile() { getCodeEditor()->compile(); }
void EditorWindow::upload()  { getCodeEditor()->upload(); }

void EditorWindow::findReplaceDialog() 
{ 
    findRepDiaWidget->setTextEdit( getCodeEditor() );
    findRepDiaWidget->show(); 
}

void EditorWindow::createToolBars()
{
    m_editorToolBar->addAction(newAct);
    m_editorToolBar->addAction(openAct);
    m_editorToolBar->addAction(saveAct);
    m_editorToolBar->addAction(saveAsAct);
    m_editorToolBar->addSeparator();

    /*m_editorToolBar->addAction(cutAct);
    m_editorToolBar->addAction(copyAct);
    m_editorToolBar->addAction(pasteAct);
    m_editorToolBar->addSeparator();
    m_editorToolBar->addAction(undoAct);
    m_editorToolBar->addAction(redoAct);*/
    m_editorToolBar->addAction(findQtAct);
    m_editorToolBar->addSeparator();
    
    m_editorToolBar->addAction(compileAct);
    m_editorToolBar->addAction(loadAct);
    m_editorToolBar->addSeparator();
    
    m_editorToolBar->addAction(debugAct);
    
    m_debuggerToolBar->addAction(stepAct);
    //m_debuggerToolBar->addAction(stepOverAct);
    m_debuggerToolBar->addAction(runAct);
    m_debuggerToolBar->addAction(pauseAct);
    m_debuggerToolBar->addAction(resetAct);
    m_debuggerToolBar->addSeparator();
    m_debuggerToolBar->addAction(stopAct);
}

void EditorWindow::readSettings()
{
    QSettings settings("PicLinux", "debuger");
    restoreGeometry(settings.value("geometry").toByteArray());
    docWidget->restoreGeometry(settings.value("docWidget/geometry").toByteArray());
    m_lastDir = settings.value("lastDir").toString();
}

void EditorWindow::writeSettings()
{
    QSettings settings("PicLinux", "debuger");
    settings.setValue( "geometry", saveGeometry() );
    settings.setValue( "docWidget/geometry", docWidget->saveGeometry() );
    settings.setValue( "lastDir", m_lastDir );
}

QString EditorWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void EditorWindow::about()
{
   QMessageBox::about(this, tr("About Application"),
            tr(""));
}
#include  "moc_editorwindow.cpp"
