#include "ImageCanvas.h"
#include "ImageDocument.h"
#include "Tools/PenTool.h"
#include "Tools/LineTool.h"
#include "Tools/SelectTool.h"
#include "Layers/SolidBackgroundLayer.h"

ImageCanvas::ImageCanvas(ImageDocument* document) :
    ImageView(document),
    background(new SolidBackgroundLayer(document_m->getSize(), Qt::green)),
    rubberBand(0)
{
    this->setBackgroundRole(QPalette::Base);

    myPenWidth = 1;
    myPenColor = Qt::black;
    scaleFactor = 1.0;
    currentTool = new PenTool(document_m, QPen(myPenColor, myPenWidth, Qt::SolidLine,
                                         Qt::RoundCap, Qt::RoundJoin));
//    currentTool = new SelectTool(document_m);
}

ImageCanvas::~ImageCanvas()
{
    delete background;
    delete currentTool;
    delete rubberBand;
}

/*
 * View Properties
 */
void ImageCanvas::resetScale()
{
    this->setScale(1.0);
}

void ImageCanvas::setScale(double new_factor)
{
    scaleFactor = new_factor;;
    //  TODO: Need to find a way to pass this up to the main window, or set it as an internal state that the main window polls
    //  zoomInAct->setEnabled(scaleFactor < 3.0);
    //  zoomOutAct->setEnabled(scaleFactor > 0.333);
    QImage* new_image = document_m->getImage();
    this->refreshImage(*new_image);
    delete new_image;
}

// don't need to let users scale by a constant factor
void ImageCanvas::scaleImage(double factor)
{
     Q_ASSERT(this->pixmap());
     this->setScale(scaleFactor*factor);
     // we need to scale the rubberBand
}

/*
 * Edit Properties
 */
void ImageCanvas::setPenColor(const QColor &new_color)
{
    myPenColor = new_color;
}

void ImageCanvas::setPenWidth(int new_width)
{
    myPenWidth = new_width;
}

void ImageCanvas::refreshImage(const QImage& image)
{
    QSize new_size = image.size()*scaleFactor;

    if (image.size() != background->size())
    {
        BackgroundLayer * replacement = background->copySized(image.size());
        delete background;
        background = replacement;
    }
    QImage new_image = ((Layer) image.copy()).layOver(background->copy());

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
    if (currentTool) {
        delete currentTool;
    }
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
    mySelection = rect;
    rubberBand->setGeometry(QRect(rect.topLeft()*scaleFactor, rect.size()*scaleFactor));
    rubberBand->show();
    emit selectionModified();
}

void ImageCanvas::showSelection(bool show)
{
    if (rubberBand)
    {
        if (show)
        {
            rubberBand->show();
            mySelection = QRect();
        }
        else
        {
            rubberBand->hide();
        }
    }
    emit selectionModified();
}

bool ImageCanvas::hasSelection()
{
    return (rubberBand && !rubberBand->isHidden());
}

QRect ImageCanvas::getSelection()
{
    return rubberBand->geometry();
}

void ImageCanvas::translateSelection(QPoint point)
{
    document_m->translateSelection(point);
    rubberBand->setGeometry(rubberBand->geometry().translated(point));
    // don't emit selectionModified
}
