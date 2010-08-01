#include "ImageCanvas.h"
#include "ImageDocument.h"
#include <iostream>
#include "SolidBackgroundLayer.h"
using namespace std;

ImageCanvas::ImageCanvas(ImageDocument* document)
    :	document_m(document),
	background(new SolidBackgroundLayer(document_m->getSize(), Qt::green)),
	scratchpad(document_m->size(), QImage::Format_ARGB32)
{
    this->setBackgroundRole(QPalette::Base);
    this->setStyleSheet("QLabel { background-color: #999999 }");

    myPenWidth = 1;
    myPenColor = Qt::black;
    scaleFactor = 1.0;
    currentTool = new PenTool(this, QPen(myPenColor, myPenWidth, Qt::SolidLine,
					 Qt::RoundCap, Qt::RoundJoin));
}

/*
 * View Properties
 */
void ImageCanvas::resetScale()
{
    this->setScale(1.0);
}

void ImageCanvas::setScale(double newFactor)
{
    scaleFactor = newFactor;;
    //  TODO: Need to find a way to pass this up to the main window, or set it as an internal state that the main window polls
    //  zoomInAct->setEnabled(scaleFactor < 3.0);
    //  zoomOutAct->setEnabled(scaleFactor > 0.333);
    this->refreshScratchpad();
    this->refreshImage(document_m->getImage());
}

// don't need to let users scale by a constant factor
void ImageCanvas::scaleImage(double factor)
{
     Q_ASSERT(this->pixmap());
     this->setScale(scaleFactor*factor);
}

/*
 * Edit Properties
 */
void ImageCanvas::setPenColor(const QColor &newColor)
{
    myPenColor = newColor;
}

void ImageCanvas::setPenWidth(int newWidth)
{
    myPenWidth = newWidth;
}

void ImageCanvas::refreshImage(const QImage& image)
{
    QSize   new_size = image.size()*scaleFactor;
    QImage  new_image = scratchpad.layOver(((Layer) image.copy()).layOver(background->copy()));

    this->setPixmap(QPixmap::fromImage(new_image.scaled(new_size)));
    this->resize(new_size);
    this->repaint();
}

void ImageCanvas::refreshScratchpad()
{
    scratchpad = Layer(document_m->size()*scaleFactor, QImage::Format_ARGB32);
}

QPoint ImageCanvas::getPoint(QMouseEvent *event)
{
    QPoint original = event->pos();
    return original/scaleFactor;
}

void ImageCanvas::mousePressEvent(QMouseEvent *event)
{
    currentTool->mousePressEvent(event);
}

void ImageCanvas::mouseMoveEvent(QMouseEvent *event)
{
    currentTool->mouseMoveEvent(event);
}

void ImageCanvas::mouseReleaseEvent(QMouseEvent *event)
{
    currentTool->mouseReleaseEvent(event);
}

/*
 * Edit Actions
 */
void ImageCanvas::drawLine(const QPoint &startPoint, const QPoint &endPoint, const QPen &pen)
{
    QPainter painter(&scratchpad);
    painter.setPen(pen);
    painter.drawLine(startPoint, endPoint);
    painter.end();

    int rad = (myPenWidth / 2) + 2;
    this->update(QRect(startPoint, endPoint).normalized().adjusted(-rad, -rad, +rad, +rad));
    this->refreshImage(document_m->getImage());
}

void ImageCanvas::commitLines(const QVector<QPoint> &pointPairs, const QPen &pen)
{
    document_m->drawLines(pen, pointPairs);
    this->refreshScratchpad();
}
