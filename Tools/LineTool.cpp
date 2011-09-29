#include "LineTool.h"
#include "ImageDocument.h"

LineTool::LineTool(ImageDocument* document)
    : document_m(document), scribbling(false)
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
    if (scribbling && (event->buttons() & Qt::LeftButton)) {
	curPoint = event->pos();
	document_m->refreshScratchpad();
        document_m->scratchLine(lastPoint, curPoint);
    }
}

void LineTool::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && scribbling) {
	// do we want to hop onto the draw lines, or make a new draw line
	QVector<QPoint> pointPairs;
	curPoint = event->pos();
	pointPairs.push_back(lastPoint);
	pointPairs.push_back(curPoint);
        document_m->drawLines(pointPairs);
	scribbling = false;
    }
}
