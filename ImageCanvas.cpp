#include "ImageCanvas.h"
#include "ImageDocument.h"
#include "PenTool.h"
#include "LineTool.h"
#include "SelectTool.h"
#include <iostream>
#include "SolidBackgroundLayer.h"
using namespace std;

ImageCanvas::ImageCanvas(ImageDocument* document) :
    ImageView(document),
    background(new SolidBackgroundLayer(document_m->getSize(), Qt::green)),
    rubberBand(0)
{
    this->setBackgroundRole(QPalette::Base);
    this->setStyleSheet("QLabel { background-color: #999999 }");

    myPenWidth = 1;
    myPenColor = Qt::black;
    scaleFactor = 1.0;
    currentTool = new PenTool(document_m, QPen(myPenColor, myPenWidth, Qt::SolidLine,
                                         Qt::RoundCap, Qt::RoundJoin));
//    currentTool = new SelectTool(document_m);
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
    QImage * newImage = document_m->getImage();
    this->refreshImage(*newImage);
    delete newImage;	
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
    QImage  new_image = ((Layer) image.copy()).layOver(background->copy());

    this->setPixmap(QPixmap::fromImage(new_image.scaled(new_size)));
    this->resize(new_size);
    this->repaint();
}

QPoint ImageCanvas::normalizePoint(QPoint point)
{
    // I suspect this may be slightly slow.
    return (point-QPoint(scaleFactor, scaleFactor)/2)/scaleFactor;
}

void ImageCanvas::mousePressEvent(QMouseEvent *event)
{
    QMouseEvent *adjustedEvent =
	    new QMouseEvent(event->type(), normalizePoint(event->pos()),event->button(),event->buttons(),event->modifiers());
    currentTool->mousePressEvent(adjustedEvent);
    delete adjustedEvent;
}

void ImageCanvas::mouseMoveEvent(QMouseEvent *event)
{
    QMouseEvent *adjustedEvent =
	    new QMouseEvent(event->type(), normalizePoint(event->pos()),event->button(),event->buttons(),event->modifiers());
    currentTool->mouseMoveEvent(adjustedEvent);
    delete adjustedEvent;
}

void ImageCanvas::mouseReleaseEvent(QMouseEvent *event)
{
    QMouseEvent *adjustedEvent =
	    new QMouseEvent(event->type(), normalizePoint(event->pos()),event->button(),event->buttons(),event->modifiers());
    currentTool->mouseReleaseEvent(adjustedEvent);
    delete adjustedEvent;
}

void ImageCanvas::setTool(Tool::ToolTypes type)
{
    switch(type)
    {
    case Tool::PenTool :
        currentTool = new PenTool(document_m, QPen(myPenColor, myPenWidth, Qt::SolidLine,
                                             Qt::RoundCap, Qt::RoundJoin));
        break;
    case Tool::LineTool :
        currentTool = new LineTool(document_m, QPen(myPenColor, myPenWidth, Qt::SolidLine,
                                             Qt::RoundCap, Qt::RoundJoin));
        break;
    case Tool::SelectTool :
        currentTool = new SelectTool(this);
        break;
    }
}

void ImageCanvas::setSelectBox(QRect rect)
{
    if (!rubberBand)
    {
        rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
    }
    rubberBand->setGeometry(rect);
    rubberBand->show();
}

void ImageCanvas::setSelectShow(bool show)
{
    if (show)
    {
        rubberBand->show();
    }
    else
    {
        rubberBand->hide();
    }
}
