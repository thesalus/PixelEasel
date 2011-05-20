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
        save_action->setEnabled(!undo_group->isClean());
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
    save_action->setEnabled(!undo_group->isClean());
}

void PixelEasel::cut()
{
    ImageDocument *doc = activeDocument();
    if (doc != 0)
        doc->cut(clipboard);
}

void PixelEasel::copy()
{
    ImageDocument *doc = activeDocument();
    if (doc != 0)
        doc->copy(clipboard);
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
        doc->clearSelection();
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
    ImageDocument *document = activeDocument();
    if (document != 0)
        zoom_widget->setValue(document->getZoomInActiveView()+1);
}

void PixelEasel::zoomOut()
{
    ImageDocument *document = activeDocument();
    if (document != 0)
        zoom_widget->setValue(document->getZoomInActiveView()-1);
}

void PixelEasel::normalSize()
{
    zoom_widget->setValue(ImageCanvas::default_scale_factor);
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
     new_action = new QAction(tr("&New..."), this);
        new_action->setShortcut(QKeySequence::New);
         connect(new_action, SIGNAL(triggered()), this, SLOT(newFile()));
     open_action = new QAction(tr("&Open..."), this);
         open_action->setShortcut(QKeySequence::Open);
        connect(open_action, SIGNAL(triggered()), this, SLOT(open()));
     save_action = new QAction(tr("&Save..."), this);
        save_action->setShortcut(QKeySequence::Save);
        save_action->setEnabled(false);
        connect(save_action, SIGNAL(triggered()), this, SLOT(save()));
     save_as_action = new QAction(tr("Save As..."), this);
        save_as_action->setShortcut(QKeySequence::SaveAs);
        save_as_action->setEnabled(false);
        connect(save_as_action, SIGNAL(triggered()), this, SLOT(saveAs()));
     exit_action = new QAction(tr("E&xit"), this);
        exit_action->setShortcut(QKeySequence::Quit);
        connect(exit_action, SIGNAL(triggered()), this, SLOT(close()));

     undo_action = undo_group->createUndoAction(this, tr("&Undo"));
        undo_action->setShortcuts(QKeySequence::Undo);
     redo_action = undo_group->createRedoAction(this, tr("&Redo"));
        redo_action->setShortcuts(QKeySequence::Redo);

     cut_action = new QAction(tr("Cu&t"), this);
        cut_action->setShortcut(QKeySequence::Cut);
        connect(cut_action, SIGNAL(triggered()), this, SLOT(cut()));
     copy_action = new QAction(tr("&Copy"), this);
        copy_action->setShortcut(QKeySequence::Copy);
        connect(copy_action, SIGNAL(triggered()), this, SLOT(copy()));
     paste_action = new QAction(tr("&Paste"), this);
        paste_action->setShortcut(QKeySequence::Paste);
        connect(paste_action, SIGNAL(triggered()), this, SLOT(paste()));
     clear_selection_action = new QAction(tr("C&lear Selection"), this);
        clear_selection_action->setShortcut(QKeySequence::Delete);
        connect(clear_selection_action, SIGNAL(triggered()), this, SLOT(clearSelection()));

     select_all_action = new QAction(tr("&Select All"), this);
        select_all_action->setShortcut(QKeySequence::SelectAll);
        connect(select_all_action, SIGNAL(triggered()), this, SLOT(selectAll()));

     edit_actions_action = new QAction(tr("&Edit Actions"), this);
        connect(edit_actions_action, SIGNAL(triggered()), this, SLOT(editActions()));
     save_actions_action = new QAction(tr("S&ave Actions"), this);
        connect(save_actions_action, SIGNAL(triggered()), this, SLOT(saveActions()));

     zoom_in_action = new QAction(tr("Zoom &In"), this);
        zoom_in_action->setShortcut(QKeySequence::ZoomIn);
        zoom_in_action->setEnabled(false);
        connect(zoom_in_action, SIGNAL(triggered()), this, SLOT(zoomIn()));
     zoom_out_action = new QAction(tr("Zoom &Out"), this);
        zoom_out_action->setShortcut(QKeySequence::ZoomOut);
        zoom_out_action->setEnabled(false);
        connect(zoom_out_action, SIGNAL(triggered()), this, SLOT(zoomOut()));
     normal_size_action = new QAction(tr("&Normal Size"), this);
        normal_size_action->setEnabled(false);
        connect(normal_size_action, SIGNAL(triggered()), this, SLOT(normalSize()));

     resize_action = new QAction(tr("&Resize Image"), this);
        resize_action->setEnabled(false);
        connect(resize_action, SIGNAL(triggered()), this, SLOT(resizeImage()));

     about_action = new QAction(tr("&About"), this);
        connect(about_action, SIGNAL(triggered()), this, SLOT(about()));
     about_qt_action = new QAction(tr("About &Qt"), this);
        connect(about_qt_action, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
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
     fileMenu->addAction(new_action);
     fileMenu->addAction(open_action);
     fileMenu->addAction(save_action);
     fileMenu->addAction(save_as_action);
     fileMenu->addSeparator();
     fileMenu->addAction(exit_action);
     menuBar()->addMenu(fileMenu);

     editMenu = new QMenu(tr("&Edit"), this);
     editMenu->addAction(undo_action);
     editMenu->addAction(redo_action);
     editMenu->addSeparator();
     editMenu->addAction(cut_action);
     editMenu->addAction(copy_action);
     editMenu->addAction(paste_action);
     editMenu->addSeparator();
     editMenu->addAction(clear_selection_action);
     editMenu->addAction(select_all_action);
     editMenu->addSeparator();
     editMenu->addAction(edit_actions_action);
     editMenu->addAction(save_actions_action);
     menuBar()->addMenu(editMenu);

     viewMenu = new QMenu(tr("&View"), this);
     viewMenu->addAction(zoom_in_action);
     viewMenu->addAction(zoom_out_action);
     viewMenu->addAction(normal_size_action);
     menuBar()->addMenu(viewMenu);

     imageMenu = new QMenu(tr("&Image"), this);
     imageMenu->addAction(resize_action);
     menuBar()->addMenu(imageMenu);

     helpMenu = new QMenu(tr("&Help"), this);
     helpMenu->addAction(about_action);
     helpMenu->addAction(about_qt_action);
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
    palette_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
    addDockWidget(Qt::BottomDockWidgetArea, palette_dock);
    palette_view = new PaletteWidget(this);
    connect(palette_view,   SIGNAL(selectedColour(PaletteColour*)),
            this,           SLOT(setColour(PaletteColour*)));
    palette_dock->setWidget(palette_view);

    view_dock = new QDockWidget(tr("View"), this);
    view_dock->setAllowedAreas(Qt::BottomDockWidgetArea);
    addDockWidget(Qt::BottomDockWidgetArea, view_dock);
    zoom_widget = new ZoomWidget(8, ImageCanvas::scale_factor_strings);
    zoom_widget->setValue(ImageCanvas::default_scale_factor);
    connect(zoom_widget,  SIGNAL(valueChanged(int)),
            this,           SLOT(zoomChanged(int)));
    view_dock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    view_dock->setWidget(zoom_widget);
    view_dock->resize(200, 200);

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
    zoom_in_action->setEnabled(hasDocument);
    zoom_out_action->setEnabled(hasDocument);
    resize_action->setEnabled(hasDocument);
    normal_size_action->setEnabled(hasDocument); // check zoom levels
    save_as_action->setEnabled(hasDocument);

    updateEditActions(this->activeDocument()->hasSelection());
}

void PixelEasel::updateEditActions(bool hasSelection)
{
    bool hasDocument = (this->activeDocument() != 0);
    bool hasClipboard = false;
    if (hasDocument)
        hasClipboard = true;    //TODO: if something is in the clipboard
    else
        hasSelection = false;
    clear_selection_action->setEnabled(hasSelection);
    cut_action->setEnabled(hasSelection);
    copy_action->setEnabled(hasSelection);
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
    if (window != 0 && old_window != window)
    {
        old_window = window;    // avoid repeated, unnecessary updates
        ImageDocument* document = (ImageDocument*) window;
        undo_group->setActiveStack(document->getUndoStack());
        document->setToolInActiveView(hotkeys->getToolType());
        ImagePreview* preview = document->getPreview();
            preview->resize(QSize(50,50));
        this->previews->setCurrentWidget(preview);
        document->refreshScratchpad();
        palette_view->changeSwatch(document->getPalette());
        zoom_widget->setValue(document->getZoomInActiveView());
    }
}

void PixelEasel::updateSave(bool save_state)
{
    save_action->setEnabled(!save_state && activeDocument()->hasFile());
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
            if (!shortcut.isNull())
                action->setShortcut(QKeySequence(shortcut.toString()));
        }
    }
}

void PixelEasel::editActions()
{
    QList<QAction *> actions = ((QObject*)this)->findChildren<QAction *>();
    QList<QAction *> list;
    QListIterator<QAction*> i(actions);
    QAction *action;
    while (i.hasNext())
    {
        action = i.next();
        if (!action->text().isEmpty())
            list.push_back(action);
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

    while (i.hasNext())
    {
        action = i.next();
        if (!action->shortcut().isEmpty())
        {
            QString shortcut = QString(action->shortcut());
            settings.setValue(action->text(), shortcut);
        }
    }
}

void PixelEasel::zoomChanged(int new_scale)
{
    if (activeDocument() != NULL)
        activeDocument()->setZoomInActiveView(new_scale);
}
