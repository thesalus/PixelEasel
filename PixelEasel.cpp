#include "PixelEasel.h"
#include "ImagePreview.h"
#include "ResizeDialog.h"
#include <QFileDialog>
#include <QDir>
#include <QToolButton>
#include <iostream>
using namespace std;

PixelEasel::PixelEasel(QWidget *parent) :
    QMainWindow(parent),
    clipboard(QApplication::clipboard())
{
    undoGroup = new QUndoGroup;

    mdiArea = new QMdiArea;
    mdiArea->setBackgroundRole(QPalette::Dark);
    mdiArea->setViewMode(QMdiArea::TabbedView);
    // TODO: find a way to add an X to close tabs.
    setCentralWidget(mdiArea);

    createActions();
    createHotkeys();
    createMenus();
    createDocks();

    setWindowTitle(tr("Pixel Easel"));
    resize(700, 500);
}

PixelEasel::~PixelEasel()
{
}

void PixelEasel::newFile()
{
    // Throw in sane defaults
    ResizeDialog* rd = new ResizeDialog(QSize(64,64), this);
    rd->setModal(true);
    QSize size;
    while (!size.isValid())
    {
	rd->exec();
	rd->activateWindow();
	size = rd->getSize();
    }
    if (activeDocument() != NULL && size != activeDocument()->getSize())
	activeDocument()->setSize(size);
    ImageDocument* imageDocument = new ImageDocument(size);
    setupContext(imageDocument);
    imageDocument->showMaximized();
    updateActions();
}

void PixelEasel::open()
{
    QFileDialog* fd = new QFileDialog(this, tr("Open File"),
                                      QDir::currentPath(),
                                      tr("Images (*.png *.gif)"));
    QString fileName = fd->getOpenFileName();
    if (!fileName.isEmpty())
    {
	QList<QMdiSubWindow*> list = mdiArea->subWindowList();
	ImageDocument* imageDocument = NULL;

        // ensure uniqueness
        for (int i = 0; i < list.size(); i++) {
            if (((ImageDocument*) list.at(i))->getPath() == fileName) {
                imageDocument = (ImageDocument*) list.at(i);
                break;
            }
        }
        if (imageDocument == NULL) {
            imageDocument = new ImageDocument(fileName);
            setupContext(imageDocument);
        }
        imageDocument->showMaximized();
        updateActions();
    }
}
      
void PixelEasel::save()
{
    if (activeDocument()->hasFile())
    {
        activeDocument()->save();
        saveAct->setEnabled(!undoGroup->isClean());
    }
}

void PixelEasel::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
				    QDir::currentPath(),
				    tr("Images (*.png *.gif)"));
    if (fileName.isEmpty() || activeDocument() == NULL)
	return;
    activeDocument()->setFileName(fileName);
    activeDocument()->save();
    saveAct->setEnabled(!undoGroup->isClean());
}

void PixelEasel::cut()
{
    ImageDocument *doc = activeDocument();
    if (doc != 0)
    {
        doc->cut(clipboard);
    }
}

void PixelEasel::copy()
{
    ImageDocument *doc = activeDocument();
    if (doc != 0)
    {
        doc->copy(clipboard);
    }
}

void PixelEasel::paste()
{
    ImageDocument *doc = activeDocument();
    if (doc != 0)
    {
        // TODO: we need to grab clipboard events, or check whenever focus returns to this window.
        // check if there is something in the clipboard
        doc->paste(clipboard);
        setTool((int)Tool::SelectTool);
        hotkeys->setTool((int) Tool::SelectTool);
    }
}

void PixelEasel::selectAll()
{
    ImageDocument *doc = activeDocument();
    if (doc != 0)
    {
        doc->setSelection(QRect(QPoint(0,0),doc->getSize()));
        setTool((int)Tool::SelectTool);
        hotkeys->setTool((int) Tool::SelectTool);
    }
}

void PixelEasel::zoomIn()
{
    activeDocument()->scaleImage(1.25);
}

void PixelEasel::zoomOut()
{
    activeDocument()->scaleImage(0.8);
}

void PixelEasel::normalSize()
{
    activeDocument()->resetScale();
}

void PixelEasel::resizeImage()
{
    ResizeDialog* rd = new ResizeDialog(activeDocument()->getSize(), this);
    rd->setModal(true);
    rd->exec();
    rd->activateWindow();
    QSize size = rd->getSize();
    if (activeDocument() != NULL && size.isValid() && size != activeDocument()->getSize())
	activeDocument()->setSize(size);
}

ImageDocument* PixelEasel::activeDocument()
{
    return (ImageDocument*) mdiArea->currentSubWindow();
}

void PixelEasel::about()
{
    QMessageBox::about(this, tr("About Pixel Easel"),
            tr("<p>Just a cross-platform pixel art editor.</p>"));
}

void PixelEasel::createActions()
 {
     newAct = new QAction(tr("&New..."), this);
     newAct->setShortcut(QKeySequence::New);
     connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));
     openAct = new QAction(tr("&Open..."), this);
     openAct->setShortcut(QKeySequence::Open);
     connect(openAct, SIGNAL(triggered()), this, SLOT(open()));
     saveAct = new QAction(tr("&Save..."), this);
     saveAct->setShortcut(QKeySequence::Save);
     saveAct->setEnabled(false);
     connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));
     saveAsAct = new QAction(tr("Save As..."), this);
     saveAsAct->setShortcut(QKeySequence::SaveAs);
     saveAsAct->setEnabled(false);
     connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));
     exitAct = new QAction(tr("E&xit"), this);
     exitAct->setShortcut(QKeySequence::Quit);
     connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

     undoAct = undoGroup->createUndoAction(this, tr("&Undo"));
     undoAct->setShortcuts(QKeySequence::Undo);
     redoAct = undoGroup->createRedoAction(this, tr("&Redo"));
     redoAct->setShortcuts(QKeySequence::Redo);

     cutAct = new QAction(tr("Cu&t"), this);
     cutAct->setShortcut(QKeySequence::Cut);
     connect(cutAct, SIGNAL(triggered()), this, SLOT(cut()));
     copyAct = new QAction(tr("&Copy"), this);
     copyAct->setShortcut(QKeySequence::Copy);
     connect(copyAct, SIGNAL(triggered()), this, SLOT(copy()));
     pasteAct = new QAction(tr("&Paste"), this);
     pasteAct->setShortcut(QKeySequence::Paste);
     connect(pasteAct, SIGNAL(triggered()), this, SLOT(paste()));

     selectAllAct = new QAction(tr("&Select All"), this);
     selectAllAct->setShortcut(QKeySequence::SelectAll);
     connect(selectAllAct, SIGNAL(triggered()), this, SLOT(selectAll()));

     zoomInAct = new QAction(tr("Zoom &In (25%)"), this);
     zoomInAct->setShortcut(QKeySequence::ZoomIn);
     zoomInAct->setEnabled(false);
     connect(zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));
     zoomOutAct = new QAction(tr("Zoom &Out (25%)"), this);
     zoomOutAct->setShortcut(QKeySequence::ZoomOut);
     zoomOutAct->setEnabled(false);
     connect(zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));
     normalSizeAct = new QAction(tr("&Normal Size"), this);
     normalSizeAct->setEnabled(false);
     connect(normalSizeAct, SIGNAL(triggered()), this, SLOT(normalSize()));

     resizeAct = new QAction(tr("&Resize Image"), this);
     resizeAct->setEnabled(false);
     connect(resizeAct, SIGNAL(triggered()), this, SLOT(resizeImage()));

     aboutAct = new QAction(tr("&About"), this);
     connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
     aboutQtAct = new QAction(tr("About &Qt"), this);
     connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
 }

void PixelEasel::createHotkeys()
{
    hotkeys = new HotkeyBar(this, 3);

    hotkeys->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
    addToolBar(Qt::BottomToolBarArea, hotkeys);
    // TODO: should we only show this if there is an active document?
}

void PixelEasel::createMenus()
{
     fileMenu = new QMenu(tr("&File"), this);
     fileMenu->addAction(newAct);
     fileMenu->addAction(openAct);
     fileMenu->addAction(saveAct);
     fileMenu->addAction(saveAsAct);
     fileMenu->addSeparator();
     fileMenu->addAction(exitAct);
     menuBar()->addMenu(fileMenu);

     editMenu = new QMenu(tr("&Edit"), this);
     editMenu->addAction(undoAct);
     editMenu->addAction(redoAct);
     editMenu->addSeparator();
     editMenu->addAction(cutAct);
     editMenu->addAction(copyAct);
     editMenu->addAction(pasteAct);
     editMenu->addSeparator();
     editMenu->addAction(selectAllAct);
     menuBar()->addMenu(editMenu);

     viewMenu = new QMenu(tr("&View"), this);
     viewMenu->addAction(zoomInAct);
     viewMenu->addAction(zoomOutAct);
     viewMenu->addAction(normalSizeAct);
     menuBar()->addMenu(viewMenu);

     imageMenu = new QMenu(tr("&Image"), this);
     imageMenu->addAction(resizeAct);
     menuBar()->addMenu(imageMenu);

     helpMenu = new QMenu(tr("&Help"), this);
     helpMenu->addAction(aboutAct);
     helpMenu->addAction(aboutQtAct);
     menuBar()->addMenu(helpMenu);
}

void PixelEasel::createDocks()
{
    previewDock = new QDockWidget(tr("Preview"), this);
    previewDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, previewDock);
    previews = new QStackedWidget();
    previewDock->setWidget(previews);

    historyDock = new QDockWidget(tr("History"), this);
    historyDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, historyDock);
    createUndoView();
    historyDock->setWidget(undoView);
}

void PixelEasel::createUndoView()
{
    undoView = new QUndoView(undoGroup, historyDock);
    // resize it somehow.
    undoView->setWindowTitle(tr("Command List"));
    undoView->show();
    undoView->setAttribute(Qt::WA_QuitOnClose, false);
}

void PixelEasel::updateActions()
{
    bool hasDocument = (this->activeDocument() != 0);
    zoomInAct->setEnabled(hasDocument);
    zoomOutAct->setEnabled(hasDocument);
    resizeAct->setEnabled(hasDocument);
    normalSizeAct->setEnabled(hasDocument); // check zoom levels
    saveAsAct->setEnabled(hasDocument);

    updateEditActions(this->activeDocument()->hasSelection());
}

void PixelEasel::updateEditActions(bool hasSelection)
{
    bool hasDocument = (this->activeDocument() != 0);
    bool hasClipboard = false;
    if (hasDocument)
    {
        hasClipboard = true;    //TODO: if something is in the clipboard
    }
    else
    {
        hasSelection = false;
    }
    cutAct->setEnabled(hasSelection);
    copyAct->setEnabled(hasSelection);
}

void PixelEasel::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value()
			    + ((factor - 1) * scrollBar->pageStep()/2)));
}

void PixelEasel::setupContext(ImageDocument* imageDocument)
{
    QUndoStack *newUndoStack = new QUndoStack();
    undoGroup->addStack(newUndoStack);
    undoGroup->setActiveStack(newUndoStack);
    imageDocument->setUndoStack(newUndoStack);
    mdiArea->addSubWindow(imageDocument);
    connect(imageDocument,  SIGNAL(imageModified(const QImage&)),
	    this,	    SLOT(updateActions()));
    connect(imageDocument,  SIGNAL(selectionModified(bool)),
            this,	    SLOT(updateEditActions(bool)));
    connect(mdiArea,	    SIGNAL(subWindowActivated(QMdiSubWindow*)),
	    this,	    SLOT(updateContext(QMdiSubWindow*)));
    connect(mdiArea,	    SIGNAL(subWindowActivated(QMdiSubWindow*)),
	    this,	    SLOT(updateContext(QMdiSubWindow*)));
    connect(undoGroup,	    SIGNAL(cleanChanged(bool)),
	    this,	    SLOT(updateSave(bool)));
    previews->insertWidget(0, imageDocument->getPreview());
    // TODO: somehow get it to remove the widget when it gets destroyed
}

void PixelEasel::updateContext(QMdiSubWindow* window)
{
    if (window != 0) {
        undoGroup->setActiveStack(((ImageDocument*) window)->getUndoStack());
        ImagePreview* preview = ((ImageDocument*) window)->getPreview();
        preview->resize(QSize(50,50));
        previews->setCurrentWidget(preview);
    }
}

void PixelEasel::updateSave(bool saveState)
{
    saveAct->setEnabled(!saveState && activeDocument()->hasFile());
}

void PixelEasel::closeEvent(QCloseEvent* e)
{
    QList<QMdiSubWindow *> list = mdiArea->subWindowList(QMdiArea::StackingOrder);
    QList<QMdiSubWindow *>::iterator i;
    for (i = list.begin(); i != list.end(); ++i)
    {
        if(!(*i)->close())
        {
            e->ignore();
            return;
        }
    }
    e->accept();
}

void PixelEasel::setTool(int type)
{
    if (activeDocument() != NULL)
        activeDocument()->setToolInActiveView((Tool::ToolTypes) type);
}
