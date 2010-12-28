#include "ImageDocument.h"
#include "Commands.h"
#include <QFileDialog>
#include <QDir>

int ImageDocument::untitled_counter = 0;

ImageDocument::ImageDocument(QSize size)
    :	fileName("Untitled"+(untitled_counter > 0 ? " "+QString::number(untitled_counter+1) : "")),
        hasFileFlag(false),
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

void ImageDocument::initialize()
{
    ImageCanvas *canvas = new ImageCanvas(this);
    connect(this,	    SIGNAL(imageModified(const QImage&)),
	    canvas,	    SLOT(refreshImage(const QImage&)));
    views << canvas;

    scrollArea = new QScrollArea;
    scrollArea->setBackgroundRole(QPalette::Base);
    scrollArea->setAlignment(Qt::AlignCenter);
    scrollArea->setStyleSheet("QLabel { background-color: #999999 }");
    scrollArea->setWidget(canvas);

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
    QImage *new_image = new QImage(scratchpad.layOver(image));
    return new_image;
}

QString ImageDocument::getPath()
{
    return fileName;
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

void ImageDocument::setFileName(QString file)
{
    fileName = file;
    hasFileFlag = true;
    makeChange();
    // TODO: we're duplicating the makechange call here when saving as...
}

void ImageDocument::setUndoStack(QUndoStack* undoStack_)
{
    undoStack = undoStack_;
    connect(undoStack,	    SIGNAL(cleanChanged(bool)),
	    this,	    SLOT(updateTitle(bool)));
}

void ImageDocument::closeEvent(QCloseEvent* event)
{
    // figure out a way to make this more convenient for users.
    if (!undoStack->isClean())
    {
        switch( QMessageBox::information( this, "Unsaved Changes in "+fileName.split("/").last(),
                                              "The document has been changed since "
                                              "the last save.",
                                              "Save Now", "Cancel", "Discard Changes",
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
                        // TODO: or should we ask them again?
                        event->ignore();
                        break;
                    }
                    setFileName(fileName);
                }
                save();
                event->accept();
                break;
            case 1:
            default:
                event->ignore();
                break;
            case 2:
                event->accept();
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
    views.first()->setTool(type);
}
