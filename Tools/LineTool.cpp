#include "LineTool.h"
#include "ImageDocument.h"

LineTool::LineTool(ImageDocument* document, QPen pen)
    : document_m(document), scribbling(false), pen_m(pen)
{
}

void LineTool::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
	lastPoint = event->pos();
	scribbling = true;
    }
    // RightButton to cancel the event?
}

void LineTool::mouseMoveEvent(QMouseEvent *event)
{
    if (scribbling && (event->buttons() & Qt::LeftButton))
    {
	curPoint = event->pos();
	document_m->refreshScratchpad();
	document_m->scratchLine(pen_m, lastPoint, curPoint);
    }
}

void LineTool::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && scribbling)
    {
	// do we want to hop onto the draw lines, or make a new draw line
	QVector<QPoint> pointPairs;
	curPoint = event->pos();
	pointPairs.push_back(lastPoint);
	pointPairs.push_back(curPoint);
	document_m->drawLines(pen_m, pointPairs);
	scribbling = false;
    }
}
