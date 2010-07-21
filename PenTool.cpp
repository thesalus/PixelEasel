#include "PenTool.h"
#include "ImageCanvas.h"
#include <iostream>
using namespace std;

PenTool::PenTool(ImageCanvas* canvas, QPen pen_)
    : imageCanvas(canvas), scribbling(false), pen(pen_)
{
}

void PenTool::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
	lastPoint = imageCanvas->getPoint(event);
	scribbling = true;
    }
}

void PenTool::mouseMoveEvent(QMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton) && scribbling) {
	QPoint nextPoint = imageCanvas->getPoint(event);
	imageCanvas->drawLine(lastPoint, nextPoint, pen);
	pointPairs.push_back(lastPoint);
	pointPairs.push_back(nextPoint);
	lastPoint = nextPoint;
    }
}

void PenTool::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && scribbling) {
	lastPoint = imageCanvas->getPoint(event);
	scribbling = false;
	imageCanvas->commitLines(pointPairs, pen);
	pointPairs.clear();
    }
}
