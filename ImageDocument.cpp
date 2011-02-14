#include "ImageDocument.h"
#include "Commands.h"
#include <QFileDialog>
#include <QDir>

int ImageDocument::untitled_counter = 0;

ImageDocument::ImageDocument(QSize size)
    :	fileName("Untitled"+(untitled_counter > 0 ? " "+QString::number(untitled_counter+1) : "")),
        hasFileFlag(false),
        selectionChanged(false),
        emptyScratchpadSelection(true),
	imageIndex(0),
	scratchpad(size, QImage::Format_ARGB32),
        undoStack(NULL)
{
    // create a transparent background
    QImage image(size, QImage::Format_ARGB32);
	image.fill(qRgba(255, 255, 255, 0));
	imageLayers << (new Layer(image));
    this->setAttribute(Qt::WA_DeleteOnClose, true);
    untitled_counter++;
    initialize();
}

ImageDocument::ImageDocument(QString fileName_)
    :	fileName(fileName_),
        hasFileFlag(true),
        selectionChanged(false),
        emptyScratchpadSelection(true),
	imageIndex(0),
	scratchpad(QSize(1,1), QImage::Format_ARGB32),
	undoStack(NULL)
{
    QImage image(fileName);
    this->setAttribute(Qt::WA_DeleteOnClose, true);
    if (image.isNull())
    {
	QMessageBox::information(this, tr("Image Canvas"),
				 tr("Cannot load %1.").arg(fileName));
	this->close();
    }
    else
    {
	scratchpad = QImage(image.size(), QImage::Format_ARGB32);
	    scratchpad.fill(qRgba(255, 255, 255, 0));
	imageLayers << (new Layer(image));
	initialize();
    }
}

// create from clipboard?

void ImageDocument::initialize()
{
    ImageCanvas *canvas = new ImageCanvas(this);
    connect(this,	    SIGNAL(imageModified(const QImage&)),
            canvas,	    SLOT(refreshImage(const QImage&)));
    connect(canvas,	    SIGNAL(selectionModified()),
            this,	    SLOT(passSelectionModified()));
    views << canvas;

    scratchpadTranslation = QPoint(0,0);

    scrollArea = new QScrollArea;
    scrollArea->setBackgroundRole(QPalette::Base);
    scrollArea->setAlignment(Qt::AlignCenter);
    scrollArea->setStyleSheet("QLabel { background-color: #999999 }");
    scrollArea->setWidget(canvas);

    preview = new ImagePreview(this);
    connect(this,	    SIGNAL(imageModified(const QImage&)),
            preview,	    SLOT(refreshImage(const QImage&)));

    this->setWidget(scrollArea);
    this->setWindowTitle(fileName.split("/").last());
    this->resize(128, 128);
    canvas->show();
    this->makeChange();
}

ImageDocument::~ImageDocument()
{
    delete undoStack;
    delete scrollArea;
}

void ImageDocument::scaleImage(double scaleFactor)
{
    // TODO: how do we do this part? separate subwindow?
    ((ImageCanvas*) views.first())->scaleImage(scaleFactor);
}

void ImageDocument::resetScale()
{
    ((ImageCanvas*) views.first())->resetScale();
}

void ImageDocument::save()
{
    // Expand this to include other file types
    // TODO: do we need to make use of hasFileFlag
    if (this->getImage()->save(fileName, "PNG"))
    {
	if (undoStack != NULL && !undoStack->isClean())
	    undoStack->setClean();
    }
    makeChange();
}

QImage* ImageDocument::getImage()
{
//    scratchpad = Layer(image.size(), QImage::Format_ARGB32);
    Layer image(scratchpad.size(),  QImage::Format_ARGB32);
    for (int i = 0; i < imageLayers.size(); ++i) {
	image = imageLayers.at(i)->layOver(image);
    }
    Layer temp(scratchpad.size(), QImage::Format_ARGB32);
    QPainter painter(&temp);
        painter.drawImage(scratchpadTranslation, scratchpad);
        painter.end();

    QImage *new_image = new QImage(temp.layOver(image));
    return new_image;
}

QString ImageDocument::getPath()
{
    return fileName;
}

ImagePreview* ImageDocument::getPreview()
{
    return preview;
}

QSize ImageDocument::getSize()
{
    // is this the best size to return?
    return scratchpad.size();
}

QUndoStack* ImageDocument::getUndoStack()
{
    return undoStack;
}

void ImageDocument::replaceImage(QImage new_image)
{
    imageLayers.replace(imageIndex, new Layer(new_image));
    this->makeChange();
}

void ImageDocument::drawImage(QImage new_image)
{
    QPainter painter(imageLayers.at(imageIndex));
    painter.drawImage(QPoint(0,0), new_image);
    painter.end();
    this->makeChange();
}

void ImageDocument::drawLines(QPen pen, QVector<QPoint> pointPairs)
{
    QImage old_image = imageLayers.at(imageIndex)->copy();
    QPainter painter(imageLayers.at(imageIndex));
	painter.setPen(pen);
	painter.drawLines(pointPairs);
	painter.end();
    if (undoStack != NULL)
    {
	AddCommand * command = new AddCommand(old_image, *imageLayers.at(imageIndex), this);
	command->setText("Draw Line");
	undoStack->push(command);
    }
    refreshScratchpad();
    this->makeChange();
}

void ImageDocument::refreshScratchpad() {
    scratchpad = Layer(imageLayers.at(imageIndex)->size(), QImage::Format_ARGB32);
    this->makeChange();
}

void ImageDocument::scratchLine(QPen pen, QPoint startPoint, QPoint endPoint)
{
    QPainter painter(&scratchpad);
	painter.setPen(pen);
	painter.drawLine(startPoint, endPoint);
	painter.end();
    //    int rad = (pen.width() / 2) + 2;
    //    this->update(QRect(startPoint, endPoint).normalized().adjusted(-rad, -rad, +rad, +rad));

    this->makeChange();
}

void ImageDocument::makeChange()
{
    emit imageModified(*getImage());
}

void ImageDocument::updateTitle(bool state)
{
    QString title = fileName.split("/").last();
    if (!state)
	title += "*";
    this->setWindowTitle(title);
}

void ImageDocument::setFileName(QString file)
{
    fileName = file;
    hasFileFlag = true;
    makeChange();
    // TODO: we're duplicating the makechange call here when saving as...
}

void ImageDocument::setSize(QSize newSize)
{
    Layer newImage(newSize, QImage::Format_RGB32);
    QPainter painter(&newImage);
    painter.drawImage(QPoint(0,0), *imageLayers.at(imageIndex));
    painter.end();
    if (undoStack != NULL)
    {
	AddCommand * command = new AddCommand(*imageLayers.at(imageIndex), newImage, this);
	command->setText("Resize Image");
	undoStack->push(command);
    }
    imageLayers.replace(imageIndex, new Layer(newImage));
    this->makeChange();
}

void ImageDocument::setUndoStack(QUndoStack* undoStack_)
{
    undoStack = undoStack_;
    connect(undoStack,	    SIGNAL(cleanChanged(bool)),
	    this,	    SLOT(updateTitle(bool)));
}

void ImageDocument::closeEvent(QCloseEvent* event)
{
    // figure out a way to make this more convenient for users when dealing with multiple files.
    if (!undoStack->isClean())
    {
        switch( QMessageBox::information( this, "Unsaved Changes in "+fileName.split("/").last(),
                                              "The document has been changed since "
                                              "the last save.",
                                              "&Save Now", "&Discard Changes", "&Cancel",
                                              0, 1 ) )
        {
            case 0:
                // if there is no filename, then query the user for one
                if (!hasFileFlag)
                {
                    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
                                                    QDir::currentPath(),
                                                    tr("Images (*.png *.gif)"));
                    if (fileName.isEmpty())
                    {
                        event->ignore();    // TODO: should we ask them again?
                        break;
                    }
                    setFileName(fileName);
                }
                save();
                event->accept();
                break;
            case 1:
                event->accept();
                break;
            case 2:
            default:
                event->ignore();
                break;
        }
    }
    else
    {
        event->accept();
    }
}

bool ImageDocument::hasFile()
{
    return hasFileFlag;
}

void ImageDocument::setToolInActiveView(Tool::ToolTypes type)
{
    if (type != Tool::SelectTool)
    {
        // what if we change the layers, do we discard the selection?
        // TODO: for each of them
        views.first()->showSelection(false);
    }
    views.first()->setTool(type);
}

bool ImageDocument::hasSelection()
{
    // look in the _ACTIVE_ view
    return views.first()->hasSelection();
}

void ImageDocument::clearRect(QRect rect)
{
    QPainter painter(imageLayers.at(imageIndex));
        painter.setBackgroundMode(Qt::TransparentMode);
        painter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
        painter.eraseRect(rect);
        painter.end();
    this->makeChange();
}

void ImageDocument::cut(QClipboard *clipboard)
{
    // TODO BUG: we need to merge the scratchpad first
    if (views.first()->hasSelection())
    {
        QImage oldImage = imageLayers.at(imageIndex)->copy(imageLayers.at(imageIndex)->rect());
        QRect selection = views.first()->getSelection();
        clipboard->setImage(imageLayers.at(imageIndex)->copy(selection));
        views.first()->showSelection(false);
        clearRect(selection);
        if (undoStack != NULL)
        {
            AddCommand * command = new AddCommand(oldImage, *imageLayers.at(imageIndex), this);
            command->setText("Cut");
            undoStack->push(command);
        }
    }
}

void ImageDocument::copy(QClipboard *clipboard)
{
    if (views.first()->hasSelection())
    {
        QRect selection = views.first()->getSelection();
        clipboard->setImage(imageLayers.at(imageIndex)->copy(selection));
    }
}

void ImageDocument::paste(QClipboard *clipboard)
{
    if(clipboard->mimeData()->hasImage())
    {
        preScratch = imageLayers.at(imageIndex)->copy(imageLayers.at(imageIndex)->rect());
        QPainter painter(&scratchpad);
            painter.drawImage(QPoint(0,0), clipboard->image());
            painter.end();
        setSelection(QRect(QPoint(0,0), clipboard->image().size()));
        emptyScratchpadSelection = false;
        // the pasting action gets recorded when they actually release the selection
    }
}

void ImageDocument::setSelection(QRect rect)
{
    views.first()->setSelectBox(rect);
    selectionChanged = true;
    emit makeChange();
}

void ImageDocument::passSelectionModified()
{
    // merge the selected image back onto the original if it was changed
    if (selectionChanged)
    {
        QPainter painter(imageLayers.at(imageIndex));
            painter.drawImage(scratchpadTranslation, scratchpad);
            painter.end();
        refreshScratchpad();
        if (undoStack != NULL)
        {
            AddCommand * command = new AddCommand(preScratch, *imageLayers.at(imageIndex), this);
            command->setText("Merge Selection");
            undoStack->push(command);
        }
        // TODO: don't put this on the stack if we cancel
        selectionChanged = false;
        emptyScratchpadSelection = true;
        scratchpadTranslation = QPoint(0,0);
    }
    emit selectionModified(hasSelection());
}

// TODO: close if the primary view is closed?

void ImageDocument::selectToScratchpad()
{
    // is it a bad idea to clip it out of the image? Yes.
    QRect rect = views.first()->getSelection();
    preScratch = imageLayers.at(imageIndex)->copy();
    QImage moved = imageLayers.at(imageIndex)->copy(rect);
    clearRect(rect);
    QPainter painter(&scratchpad);
        painter.setBackgroundMode(Qt::TransparentMode);
        painter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
        painter.eraseRect(scratchpad.rect());
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        painter.drawImage(rect.topLeft(), moved);
        painter.end();
    emptyScratchpadSelection = false;
    selectionChanged = true;
    emit makeChange();
}

void ImageDocument::translateSelection(QPoint point)
{
    if (emptyScratchpadSelection)
    {
        selectToScratchpad();
    }
    scratchpadTranslation += point;
    emit makeChange();
}
