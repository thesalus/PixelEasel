#include "ImageDocument.h"
#include "Commands.h"
#include <iostream>
using namespace std;

ImageDocument::ImageDocument(QSize size)
    :	fileName("Untitled"),
	image(size, QImage::Format_ARGB32),
	scratchpad(size, QImage::Format_ARGB32),
	undoStack(NULL)
{
    image.fill(qRgba(255, 255, 255, 0));
    this->setAttribute(Qt::WA_DeleteOnClose, true);
    initialize();
}

ImageDocument::ImageDocument(QString fileName_)
    :	fileName(fileName_),
	image(fileName),
	scratchpad(image.size(), QImage::Format_ARGB32),
	undoStack(NULL)
{
    this->setAttribute(Qt::WA_DeleteOnClose, true);
    if (image.isNull()) {
	QMessageBox::information(this, tr("Image Canvas"),
				 tr("Cannot load %1.").arg(fileName));
	this->close();
    } else {
	initialize();
    }
}

void ImageDocument::initialize()
{
    canvas = new ImageCanvas(this);

    connect(this,	    SIGNAL(imageModified(const QImage&)),
	    canvas,	    SLOT(refreshImage(const QImage&)));

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
    delete canvas;
    delete scrollArea;
}

void ImageDocument::scaleImage(double scaleFactor)
{
    canvas->scaleImage(scaleFactor);
}

void ImageDocument::resetScale()
{
    canvas->resetScale();
}

void ImageDocument::save(QString file)
{
    if (file.isEmpty())
	file = fileName;
    if (this->image.save(file, "PNG")) {
	if (undoStack != NULL && !undoStack->isClean())
	    undoStack->setClean();
	fileName = file;
    }
    makeChange();
}

QImage* ImageDocument::getImage()
{
//    scratchpad = Layer(image.size(), QImage::Format_ARGB32);
    QImage *new_image = new QImage(scratchpad.layOver(((Layer) image.copy())));
    return new_image;
}

QString ImageDocument::getPath()
{
    return fileName;
}

QSize ImageDocument::getSize()
{
    return image.size();
}

QUndoStack* ImageDocument::getUndoStack()
{
    return undoStack;
}

void ImageDocument::replaceImage(QImage new_image)
{
    image = new_image;
    this->makeChange();
}

void ImageDocument::drawImage(QImage new_image)
{
    QPainter painter(&image);
    painter.drawImage(QPoint(0,0), new_image);
    painter.end();
    this->makeChange();
}

void ImageDocument::drawLines(QPen pen, QVector<QPoint> pointPairs)
{
    QImage old_image = image.copy();
    QPainter painter(&image);
    painter.setPen(pen);
    painter.drawLines(pointPairs);
    painter.end();
    if (undoStack != NULL)
    {
	AddCommand * command = new AddCommand(old_image, image, this);
	command->setText("Draw Line");
	undoStack->push(command);
    }
    scratchpad = Layer(image.size(), QImage::Format_ARGB32);
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
    emit imageModified(scratchpad.layOver(((Layer) image.copy())));
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
    painter.drawImage(QPoint(0,0),image);
    painter.end();
    if (undoStack != NULL)
    {
	AddCommand * command = new AddCommand(image, newImage, this);
	command->setText("Resize Image");
	undoStack->push(command);
    }
    image = newImage;
    this->makeChange();
}

void ImageDocument::setUndoStack(QUndoStack* undoStack_)
{
    undoStack = undoStack_;
    connect(undoStack,	    SIGNAL(cleanChanged(bool)),
	    this,	    SLOT(updateTitle(bool)));
}
