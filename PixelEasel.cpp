#include "PixelEasel.h"
#include "ResizeDialog.h"
#include <iostream>
using namespace std;

PixelEasel::PixelEasel(QWidget *parent) :
    QMainWindow(parent)
{
    undoGroup = new QUndoGroup;

    mdiArea = new QMdiArea;
    mdiArea->setBackgroundRole(QPalette::Dark);
    setCentralWidget(mdiArea);

    dock = new QDockWidget(tr("Actions"), this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, dock);

    createActions();
    createMenus();
    createUndoView();

    setWindowTitle(tr("Pixel Easel"));
    resize(500, 400);
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
    if (activeDocument() != NULL && size != activeDocument()->getSize())
	activeDocument()->setSize(size);
    ImageDocument* imageDocument = new ImageDocument(size);
    setupContext(imageDocument);
    imageDocument->showMaximized();
    updateActions();
}

void PixelEasel::open()
{
    QFileDialog* fd = new QFileDialog(this, tr("Open File"), QDir::currentPath(), tr("Images (*.png *.gif)"));
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
    activeDocument()->save();
    saveAct->setEnabled(!undoGroup->isClean());
}

void PixelEasel::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
				    QDir::currentPath(),
				    tr("Images (*.png *.gif)"));
    if (fileName.isEmpty() || activeDocument() == NULL)
	return;
    activeDocument()->save(fileName);
    saveAct->setEnabled(!undoGroup->isClean());
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
	    tr("<p>Foo bar!</p>"));
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

void PixelEasel::createUndoView()
{
    undoView = new QUndoView(undoGroup, dock);
    undoView->setWindowTitle(tr("Command List"));
    undoView->show();
    undoView->setAttribute(Qt::WA_QuitOnClose, false);
    dock->setWidget(undoView);
}

void PixelEasel::updateActions()
{
    bool hasDocument = (this->activeDocument() != 0);
    zoomInAct->setEnabled(hasDocument);
    zoomOutAct->setEnabled(hasDocument);
    resizeAct->setEnabled(hasDocument);
    normalSizeAct->setEnabled(hasDocument); // check zoom levels
    saveAsAct->setEnabled(hasDocument);
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
    connect(mdiArea,	    SIGNAL(subWindowActivated(QMdiSubWindow*)),
	    this,	    SLOT(updateContext(QMdiSubWindow*)));
    connect(mdiArea,	    SIGNAL(subWindowActivated(QMdiSubWindow*)),
	    this,	    SLOT(updateContext(QMdiSubWindow*)));
    connect(undoGroup,	    SIGNAL(cleanChanged(bool)),
	    this,	    SLOT(updateSave(bool)));
}

void PixelEasel::updateContext(QMdiSubWindow* window)
{
    if (window != 0)
	undoGroup->setActiveStack(((ImageDocument*) window)->getUndoStack());
}

void PixelEasel::updateSave(bool saveState)
{
    saveAct->setEnabled(!saveState);
}
