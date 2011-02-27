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
    undo_group = new QUndoGroup;

    mdi_area = new QMdiArea;
    mdi_area->setBackgroundRole(QPalette::Dark);
    mdi_area->setViewMode(QMdiArea::TabbedView);
    // TODO: find a way to add an X to close tabs.
    setCentralWidget(mdi_area);

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
        QList<QMdiSubWindow*> list = mdi_area->subWindowList();
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
        saveAct->setEnabled(!undo_group->isClean());
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
    saveAct->setEnabled(!undo_group->isClean());
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

void PixelEasel::clearSelection()
{
    ImageDocument *doc = activeDocument();
    if (doc != 0)
    {
        doc->clearSelection();
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
    return (ImageDocument*) mdi_area->currentSubWindow();
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

     undoAct = undo_group->createUndoAction(this, tr("&Undo"));
     undoAct->setShortcuts(QKeySequence::Undo);
     redoAct = undo_group->createRedoAction(this, tr("&Redo"));
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
     clearSelectionAct = new QAction(tr("C&lear Selection"), this);
     clearSelectionAct->setShortcut(QKeySequence::Delete);
     connect(clearSelectionAct, SIGNAL(triggered()), this, SLOT(clearSelection()));

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
     editMenu->addAction(clearSelectionAct);
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
    preview_dock = new QDockWidget(tr("Preview"), this);
    preview_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, preview_dock);
    previews = new QStackedWidget();
    preview_dock->setWidget(previews);

    history_dock = new QDockWidget(tr("History"), this);
    history_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, history_dock);
    createUndoView();
    history_dock->setWidget(undo_view);

    palette_dock = new QDockWidget(tr("Palette"), this);
    palette_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, palette_dock);
    palette_view = new PaletteWidget(this);
    connect(palette_view,   SIGNAL(selectedColour(PaletteColour*)),
            this,           SLOT(setColour(PaletteColour*)));
    palette_dock->setWidget(palette_view);

    viewMenu->addSeparator();
    viewMenu->addAction(preview_dock->toggleViewAction());
    viewMenu->addAction(history_dock->toggleViewAction());
    viewMenu->addAction(palette_dock->toggleViewAction());
}

void PixelEasel::createUndoView()
{
    undo_view = new QUndoView(undo_group, history_dock);
    // resize it somehow.
    undo_view->setWindowTitle(tr("Command List"));
    undo_view->show();
    undo_view->setAttribute(Qt::WA_QuitOnClose, false);
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
    clearSelectionAct->setEnabled(hasSelection);
    cutAct->setEnabled(hasSelection);
    copyAct->setEnabled(hasSelection);
}

void PixelEasel::adjustScrollBar(QScrollBar *scroll_bar, double factor)
{
    scroll_bar->setValue(int(factor * scroll_bar->value()
                            + ((factor - 1) * scroll_bar->pageStep()/2)));
}

void PixelEasel::setupContext(ImageDocument* imageDocument)
{
    QUndoStack *newUndoStack = new QUndoStack();
    undo_group->addStack(newUndoStack);
    undo_group->setActiveStack(newUndoStack);
    imageDocument->setUndoStack(newUndoStack);
    mdi_area->addSubWindow(imageDocument);
    connect(imageDocument,  SIGNAL(imageModified(const QImage&)),
	    this,	    SLOT(updateActions()));
    connect(imageDocument,  SIGNAL(selectionModified(bool)),
            this,	    SLOT(updateEditActions(bool)));
    connect(mdi_area,	    SIGNAL(subWindowActivated(QMdiSubWindow*)),
            this,	    SLOT(updateContext(QMdiSubWindow*)));
    connect(undo_group,	    SIGNAL(cleanChanged(bool)),
            this,	    SLOT(updateSave(bool)));
    previews->insertWidget(0, imageDocument->getPreview());
    // TODO: somehow get it to remove the widget when it gets destroyed
}

void PixelEasel::updateContext(QMdiSubWindow* window)
{
    if (window != 0 && old_window != window) {
        old_window = window;    // avoid repeated, unnecessary updates
        ImageDocument* document = (ImageDocument*) window;
        undo_group->setActiveStack(document->getUndoStack());
        document->setToolInActiveView(hotkeys->getToolType());
        ImagePreview* preview = document->getPreview();
            preview->resize(QSize(50,50));
        this->previews->setCurrentWidget(preview);
        document->refreshScratchpad();
        palette_view->changeSwatch(document->getPalette());
    }
}

void PixelEasel::updateSave(bool save_state)
{
    saveAct->setEnabled(!save_state && activeDocument()->hasFile());
}

void PixelEasel::closeEvent(QCloseEvent* e)
{
    QList<QMdiSubWindow *> list = mdi_area->subWindowList(QMdiArea::StackingOrder);
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

void PixelEasel::setColour(PaletteColour* colour)
{
    if (activeDocument() != NULL)
        activeDocument()->setColour(colour);
}
