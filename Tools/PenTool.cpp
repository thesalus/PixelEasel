#include "PenTool.h"
#include "ImageDocument.h"

PenTool::PenTool(ImageDocument* document)
    : document_m(document), scribbling(false)
{
}

void PenTool::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
	lastPoint = event->pos();
	scribbling = true;
    }
}

void PenTool::mouseMoveEvent(QMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton) && scribbling)
    {
	QPoint nextPoint = event->pos();
        document_m->scratchLine(lastPoint, nextPoint);
	pointPairs.push_back(lastPoint);
	pointPairs.push_back(nextPoint);
	lastPoint = nextPoint;
    }
}

void PenTool::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && scribbling)
    {
	lastPoint = event->pos();
	scribbling = false;
        document_m->drawLines(pointPairs);
	pointPairs.clear();
    }
}
