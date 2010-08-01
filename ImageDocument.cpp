#include "ImageDocument.h"
#include "Commands.h"
#include <iostream>
using namespace std;

ImageDocument::ImageDocument(QSize size, QUndoStack* undoStack_)
    : fileName("Untitled"), image(size, QImage::Format_ARGB32), undoStack(undoStack_)
{
    image.fill(qRgba(255, 255, 255, 0));
    this->setAttribute(Qt::WA_DeleteOnClose, true);
    initialize();
}

ImageDocument::ImageDocument(QString fileName_, QUndoStack* undoStack_)
    : fileName(fileName_), image(fileName), undoStack(undoStack_)
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

void ImageDocument::initialize() {
    canvas = new ImageCanvas(this);

    connect(this,   SIGNAL(imageModified(const QImage&)),
	    canvas, SLOT(refreshImage(const QImage&)));

    scrollArea = new QScrollArea;
    scrollArea->setBackgroundRole(QPalette::Base);
    scrollArea->setAlignment(Qt::AlignCenter);
    scrollArea->setStyleSheet("QLabel { background-color: #999999 }");
    scrollArea->setWidget(canvas);

    this->setWidget(scrollArea);
    this->setWindowTitle(fileName.split("/").last());
    this->resize(128, 128);
    canvas->show();
    makeChange();
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
	if (!undoStack->isClean())
	    undoStack->setClean();
	fileName = file;
    }
    makeChange();
}

const QImage & ImageDocument::getImage()
{
    return image;
}

QString ImageDocument::getPath()
{
    return fileName;
}

QSize ImageDocument::getSize()
{
    return image.size();
}

void ImageDocument::setSize(QSize newSize)
{
    Layer newImage(newSize, QImage::Format_RGB32);
    QPainter painter(&newImage);
    painter.drawImage(QPoint(0,0),image);
    painter.end();
    AddCommand * command = new AddCommand(image, newImage, this);
    command->setText("Resize Image");
    undoStack->push(command);
    image = newImage;
    makeChange();
}

void ImageDocument::replaceImage(QImage new_image)
{
    image = new_image;
    makeChange();
}

void ImageDocument::drawImage(QImage new_image)
{
    QPainter painter(&image);
    painter.drawImage(QPoint(0,0), new_image);
    painter.end();
    makeChange();
}

void ImageDocument::drawLines(QPen pen, QVector<QPoint> pointPairs)
{
    QImage old_image = image.copy();
    QPainter painter(&image);
    painter.setPen(pen);
    painter.drawLines(pointPairs);
    painter.end();
    AddCommand * command = new AddCommand(old_image, image, this);
    command->setText("Draw Line");
    undoStack->push(command);
    makeChange();
}

void ImageDocument::makeChange()
{
    QString title = fileName.split("/").last();
    if (!undoStack->isClean())
	title += "*";
    this->setWindowTitle(title);
    emit imageModified(image);
}
