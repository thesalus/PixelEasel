#include "ImageCanvas.h"
#include "ImageDocument.h"
#include "Tools/EyedropTool.h"
#include "Tools/PenTool.h"
#include "Tools/LineTool.h"
#include "Tools/SelectTool.h"
#include "Layers/SolidBackgroundLayer.h"

int ImageCanvas::defaultScaleFactor = 3;
double ImageCanvas::scaleFactors[] = {10, 25, 50, 100, 200, 400, 800, 1600};
QString ImageCanvas::scaleFactorStrings[] = {"10%","25%","50%","100%","200%","400%","800%","1600%"};

ImageCanvas::ImageCanvas(ImageDocument* document)
    : ImageView(document)
    , background(new SolidBackgroundLayer(document_m->getSize(), Qt::green))
    , m_scale(defaultScaleFactor)
    , rubberBand(0)
{
    this->setBackgroundRole(QPalette::Base);
    currentTool = new PenTool(document_m);
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
    this->setScale(defaultScaleFactor);
}

int ImageCanvas::getScale()
{
    return m_scale;
}

void ImageCanvas::setScale(int new_factor)
{
    if (new_factor > 7) {
        new_factor = 7;
    }
    else if (new_factor < 0) {
        new_factor = 0;
    }
    m_scale = new_factor;
    QImage* new_image = document_m->getImage();
    this->refreshImage(*new_image);
    // TODO: we need to scale the rubberBand
    delete new_image;
}

void ImageCanvas::refreshImage(const QImage& image)
{
    QSize new_size = image.size()*scaleFactors[m_scale]/100;

    if (image.size() != background->size()) {
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
    return (point-QPoint(scaleFactors[m_scale]/100.0, scaleFactors[m_scale]/100.0)/2)*100.0/scaleFactors[m_scale];
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
    switch(type) {
    case Tool::PenTool :
        currentTool = new PenTool(document_m);
        break;
    case Tool::LineTool :
        currentTool = new LineTool(document_m);
        break;
    case Tool::SelectTool :
        currentTool = new SelectTool(this);
        break;
    case Tool::EyedropTool :
        currentTool = new EyedropTool(document_m);
        break;
    }
}

void ImageCanvas::setSelectBox(QRect rect)
{
    if (!rubberBand) {
        rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
    }
    selection = rect;
    rubberBand->setGeometry(QRect(rect.topLeft()*scaleFactors[m_scale]/100,
                                  rect.size()*scaleFactors[m_scale]/100));
    rubberBand->show();
    emit selectionModified();
}

void ImageCanvas::showSelection(bool show)
{
    if (rubberBand) {
        if (show) {
            rubberBand->show();
            selection = QRect();
        }
        else {
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
