#include "ActionsDialog.h"
#include "PixelEasel.h"
#include "ImagePreview.h"
#include "ResizeDialog.h"
#include <QFileDialog>
#include <QDir>
#include <QSettings>
#include <QToolButton>

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
    loadActions();
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
    while (!size.isValid()) {
	rd->exec();
	rd->activateWindow();
	size = rd->getSize();
    }
    if (activeDocument() != NULL && size != activeDocument()->getSize()) {
	activeDocument()->setSize(size);
    }
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
    if (!fileName.isEmpty()) {
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
    if (activeDocument()->hasFile()) {
        activeDocument()->save();
        saveAction->setEnabled(!undoGroup->isClean());
    }
}

void PixelEasel::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
				    QDir::currentPath(),
				    tr("Images (*.png *.gif)"));
    if (fileName.isEmpty() || activeDocument() == NULL) {
	return;
    }
    activeDocument()->setFileName(fileName);
    activeDocument()->save();
    saveAction->setEnabled(!undoGroup->isClean());
}

void PixelEasel::cut()
{
    ImageDocument *document = activeDocument();
    if (document != 0) {
        document->cut(clipboard);
    }
}

void PixelEasel::copy()
{
    ImageDocument *document = activeDocument();
    if (document != 0) {
        document->copy(clipboard);
    }
}

void PixelEasel::paste()
{
    ImageDocument *document = activeDocument();
    if (document != 0) {
        // TODO: we need to grab clipboard events, or check whenever focus returns to this window.
        // check if there is something in the clipboard
        document->paste(clipboard);
        setTool((int)Tool::SelectTool);
        hotkeys->setTool((int) Tool::SelectTool);
    }
}

void PixelEasel::clearSelection()
{
    ImageDocument *document = activeDocument();
    if (document != 0) {
        document->clearSelection();
    }
}

void PixelEasel::selectAll()
{
    ImageDocument *document = activeDocument();
    if (document != 0) {
        document->setSelection(QRect(QPoint(0,0),document->getSize()));
        setTool((int)Tool::SelectTool);
        hotkeys->setTool((int) Tool::SelectTool);
    }
}

void PixelEasel::zoomIn()
{
    ImageDocument *document = activeDocument();
    if (document != 0) {
        zoomWidget->setValue(document->getZoomInActiveView()+1);
    }
}

void PixelEasel::zoomOut()
{
    ImageDocument *document = activeDocument();
    if (document != 0) {
        zoomWidget->setValue(document->getZoomInActiveView()-1);
    }
}

void PixelEasel::normalSize()
{
    zoomWidget->setValue(ImageCanvas::defaultScaleFactor);
}

void PixelEasel::resizeImage()
{
    ResizeDialog* rd = new ResizeDialog(activeDocument()->getSize(), this);
        rd->setModal(true);
        rd->exec();
        rd->activateWindow();
    QSize size = rd->getSize();
    if (activeDocument() != NULL && size.isValid() && size != activeDocument()->getSize()) {
	activeDocument()->setSize(size);
    }
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
     newAction = new QAction(tr("&New..."), this);
        newAction->setShortcut(QKeySequence::New);
         connect(newAction, SIGNAL(triggered()), this, SLOT(newFile()));
     openAction = new QAction(tr("&Open..."), this);
         openAction->setShortcut(QKeySequence::Open);
        connect(openAction, SIGNAL(triggered()), this, SLOT(open()));
     saveAction = new QAction(tr("&Save..."), this);
        saveAction->setShortcut(QKeySequence::Save);
        saveAction->setEnabled(false);
        connect(saveAction, SIGNAL(triggered()), this, SLOT(save()));
     saveAsAction = new QAction(tr("Save As..."), this);
        saveAsAction->setShortcut(QKeySequence::SaveAs);
        saveAsAction->setEnabled(false);
        connect(saveAsAction, SIGNAL(triggered()), this, SLOT(saveAs()));
     exitAction = new QAction(tr("E&xit"), this);
        exitAction->setShortcut(QKeySequence::Quit);
        connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

     undoAction = undoGroup->createUndoAction(this, tr("&Undo"));
        undoAction->setShortcuts(QKeySequence::Undo);
     redoAction = undoGroup->createRedoAction(this, tr("&Redo"));
        redoAction->setShortcuts(QKeySequence::Redo);

     cutAction = new QAction(tr("Cu&t"), this);
        cutAction->setShortcut(QKeySequence::Cut);
        connect(cutAction, SIGNAL(triggered()), this, SLOT(cut()));
     copyAction = new QAction(tr("&Copy"), this);
        copyAction->setShortcut(QKeySequence::Copy);
        connect(copyAction, SIGNAL(triggered()), this, SLOT(copy()));
     pasteAction = new QAction(tr("&Paste"), this);
        pasteAction->setShortcut(QKeySequence::Paste);
        connect(pasteAction, SIGNAL(triggered()), this, SLOT(paste()));
     clearSelectionAction = new QAction(tr("C&lear Selection"), this);
        clearSelectionAction->setShortcut(QKeySequence::Delete);
        connect(clearSelectionAction, SIGNAL(triggered()), this, SLOT(clearSelection()));

     selectAllAction = new QAction(tr("&Select All"), this);
        selectAllAction->setShortcut(QKeySequence::SelectAll);
        connect(selectAllAction, SIGNAL(triggered()), this, SLOT(selectAll()));

     editActionsAction = new QAction(tr("&Edit Actions"), this);
        connect(editActionsAction, SIGNAL(triggered()), this, SLOT(editActions()));
     saveActionsAction = new QAction(tr("S&ave Actions"), this);
        connect(saveActionsAction, SIGNAL(triggered()), this, SLOT(saveActions()));

     zoomInAction = new QAction(tr("Zoom &In"), this);
        zoomInAction->setShortcut(QKeySequence::ZoomIn);
        zoomInAction->setEnabled(false);
        connect(zoomInAction, SIGNAL(triggered()), this, SLOT(zoomIn()));
     zoomOutAction = new QAction(tr("Zoom &Out"), this);
        zoomOutAction->setShortcut(QKeySequence::ZoomOut);
        zoomOutAction->setEnabled(false);
        connect(zoomOutAction, SIGNAL(triggered()), this, SLOT(zoomOut()));
     normalSizeAction = new QAction(tr("&Normal Size"), this);
        normalSizeAction->setEnabled(false);
        connect(normalSizeAction, SIGNAL(triggered()), this, SLOT(normalSize()));

     resizeAction = new QAction(tr("&Resize Image"), this);
        resizeAction->setEnabled(false);
        connect(resizeAction, SIGNAL(triggered()), this, SLOT(resizeImage()));

     aboutAction = new QAction(tr("&About"), this);
        connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));
     aboutQtAction = new QAction(tr("About &Qt"), this);
        connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
 }

void PixelEasel::createHotkeys()
{
    hotkeys = new HotkeyBar(this, 4);

    hotkeys->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
    addToolBar(Qt::TopToolBarArea, hotkeys);
    // TODO: should we only show this if there is an active document?
}

void PixelEasel::createMenus()
{
     fileMenu = new QMenu(tr("&File"), this);
     fileMenu->addAction(newAction);
     fileMenu->addAction(openAction);
     fileMenu->addAction(saveAction);
     fileMenu->addAction(saveAsAction);
     fileMenu->addSeparator();
     fileMenu->addAction(exitAction);
     menuBar()->addMenu(fileMenu);

     editMenu = new QMenu(tr("&Edit"), this);
     editMenu->addAction(undoAction);
     editMenu->addAction(redoAction);
     editMenu->addSeparator();
     editMenu->addAction(cutAction);
     editMenu->addAction(copyAction);
     editMenu->addAction(pasteAction);
     editMenu->addSeparator();
     editMenu->addAction(clearSelectionAction);
     editMenu->addAction(selectAllAction);
     editMenu->addSeparator();
     editMenu->addAction(editActionsAction);
     editMenu->addAction(saveActionsAction);
     menuBar()->addMenu(editMenu);

     viewMenu = new QMenu(tr("&View"), this);
     viewMenu->addAction(zoomInAction);
     viewMenu->addAction(zoomOutAction);
     viewMenu->addAction(normalSizeAction);
     menuBar()->addMenu(viewMenu);

     imageMenu = new QMenu(tr("&Image"), this);
     imageMenu->addAction(resizeAction);
     menuBar()->addMenu(imageMenu);

     helpMenu = new QMenu(tr("&Help"), this);
     helpMenu->addAction(aboutAction);
     helpMenu->addAction(aboutQtAction);
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

    paletteDock = new QDockWidget(tr("Palette"), this);
    paletteDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
    addDockWidget(Qt::BottomDockWidgetArea, paletteDock);
    paletteView = new PaletteWidget(this);
    connect(paletteView,   SIGNAL(colourSelected(PaletteColour*)),
            this,           SLOT(setColour(PaletteColour*)));
    paletteDock->setWidget(paletteView);

    viewDock = new QDockWidget(tr("View"), this);
    viewDock->setAllowedAreas(Qt::BottomDockWidgetArea);
    addDockWidget(Qt::BottomDockWidgetArea, viewDock);
    zoomWidget = new ZoomWidget(8, ImageCanvas::scaleFactorStrings);
    zoomWidget->setValue(ImageCanvas::defaultScaleFactor);
    connect(zoomWidget,  SIGNAL(valueChanged(int)),
            this,           SLOT(zoomChanged(int)));
    viewDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    viewDock->setWidget(zoomWidget);
    viewDock->resize(200, 200);

    viewMenu->addSeparator();
    viewMenu->addAction(previewDock->toggleViewAction());
    viewMenu->addAction(historyDock->toggleViewAction());
    viewMenu->addAction(paletteDock->toggleViewAction());
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
    zoomInAction->setEnabled(hasDocument);
    zoomOutAction->setEnabled(hasDocument);
    resizeAction->setEnabled(hasDocument);
    normalSizeAction->setEnabled(hasDocument); // check zoom levels
    saveAsAction->setEnabled(hasDocument);

    updateEditActions(this->activeDocument()->hasSelection());
}

void PixelEasel::updateEditActions(bool hasSelection)
{
    bool hasDocument = (this->activeDocument() != 0);
    bool hasClipboard = false;
    if (hasDocument) {
        hasClipboard = true;    //TODO: if something is in the clipboard
    }
    else {
        hasSelection = false;
    }
    clearSelectionAction->setEnabled(hasSelection);
    cutAction->setEnabled(hasSelection);
    copyAction->setEnabled(hasSelection);
}

void PixelEasel::adjustScrollBar(QScrollBar *scroll_bar, double factor)
{
    scroll_bar->setValue(int(factor * scroll_bar->value()
                            + ((factor - 1) * scroll_bar->pageStep()/2)));
}

void PixelEasel::setupContext(ImageDocument* imageDocument)
{
    QUndoStack *newUndoStack = new QUndoStack();
    undoGroup->addStack(newUndoStack);
    undoGroup->setActiveStack(newUndoStack);
    imageDocument->setUndoStack(newUndoStack);
    mdiArea->addSubWindow(imageDocument);
    connect(imageDocument,  SIGNAL(imageModified(const QImage&)),
            this,	    SLOT  (updateActions()));
    connect(imageDocument,  SIGNAL(selectionModified(bool)),
            this,	    SLOT  (updateEditActions(bool)));
    connect(mdiArea,	    SIGNAL(subWindowActivated(QMdiSubWindow*)),
            this,	    SLOT  (updateContext(QMdiSubWindow*)));
    connect(undoGroup,	    SIGNAL(cleanChanged(bool)),
            this,	    SLOT  (updateSave(bool)));
    previews->insertWidget(0, imageDocument->getPreview());
    // TODO: somehow get it to remove the widget when it gets destroyed
}

void PixelEasel::updateContext(QMdiSubWindow* window)
{
    if (window != 0 && oldWindow != window) {
        oldWindow = window;    // avoid repeated, unnecessary updates
        ImageDocument* document = (ImageDocument*) window;
        undoGroup->setActiveStack(document->getUndoStack());
        document->setToolInActiveView(hotkeys->getToolType());
        ImagePreview* preview = document->getPreview();
            preview->resize(QSize(50,50));
        this->previews->setCurrentWidget(preview);
        document->refreshScratchpad();
        paletteView->changeSwatch(document->getPalette());
        zoomWidget->setValue(document->getZoomInActiveView());
    }
}

void PixelEasel::updateSave(bool save_state)
{
    saveAction->setEnabled(!save_state && activeDocument()->hasFile());
}

void PixelEasel::closeEvent(QCloseEvent* e)
{
    QList<QMdiSubWindow *> list = mdiArea->subWindowList(QMdiArea::StackingOrder);
    QList<QMdiSubWindow *>::iterator i;
    for (i = list.begin(); i != list.end(); ++i) {
        if(!(*i)->close()) {
            e->ignore();
            return;
        }
    }
    e->accept();
}

void PixelEasel::setTool(int type)
{
    if (activeDocument() != NULL) {
        activeDocument()->setToolInActiveView((Tool::ToolTypes) type);
    }
}

void PixelEasel::setColour(PaletteColour* colour)
{
    if (activeDocument() != NULL) {
        activeDocument()->setColour(colour);
    }
}

void PixelEasel::loadActions()
{
    QSettings settings("trolltech.com", "Action");
    settings.beginGroup("/Action");

    QList<QAction*> actions = ((QObject*)this)->findChildren<QAction *>();

    QListIterator<QAction*> i(actions);
    QAction *action;

    while (i.hasNext())
    {
        action = i.next();
        if (!action->text().isEmpty()) {
            QVariant shortcut = settings.value(action->text());
            if (!shortcut.isNull()) {
                action->setShortcut(QKeySequence(shortcut.toString()));
            }
        }
    }
}

void PixelEasel::editActions()
{
    QList<QAction *> actions = ((QObject*)this)->findChildren<QAction *>();
    QList<QAction *> list;
    QListIterator<QAction*> i(actions);
    QAction *action;
    while (i.hasNext()) {
        action = i.next();
        if (!action->text().isEmpty()) {
            list.push_back(action);
        }
    }
    ActionsDialog actions_dialog(list, this);
    actions_dialog.exec();
}

void PixelEasel::saveActions()
{
    QSettings settings("trolltech.com", "Action");
    settings.beginGroup("/Action");

    QList<QAction*> actions = ((QObject*)this)->findChildren<QAction *>();
    QListIterator<QAction*> i(actions);
    QAction *action;

    while (i.hasNext()) {
        action = i.next();
        if (!action->shortcut().isEmpty()) {
            QString shortcut = QString(action->shortcut());
            settings.setValue(action->text(), shortcut);
        }
    }
}

void PixelEasel::zoomChanged(int new_scale)
{
    if (activeDocument() != NULL) {
        activeDocument()->setZoomInActiveView(new_scale);
    }
}
