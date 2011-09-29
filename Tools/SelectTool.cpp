#include "SelectTool.h"
#include "ImageDocument.h"
#include <QMessageBox>

SelectTool::SelectTool(ImageCanvas* canvas_)
    : canvas(canvas_), dragging(false), selecting(false)
{
}

void SelectTool::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        origin = boundPoint(event->pos());
        if (canvas->hasSelection() && canvas->getSelection().contains(origin)) {
            midPoint = origin;
            oldCursor = canvas->cursor();
            canvas->setCursor(QCursor(Qt::SizeAllCursor));
            dragging = true;
        }
        else {
            canvas->setSelectBox(QRect(origin, QSize(1, 1)));
            selecting = true;
        }
    }
    else if (event->button() == Qt::RightButton && !dragging) {
        // TODO: we get a merge selection action... we don't want that
        canvas->translateSelection(origin - midPoint);
        canvas->showSelection(false);
    }
}

void SelectTool::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        QPoint point = boundPoint(event->pos());

        if (selecting) {
            canvas->setSelectBox(QRect(origin, point).normalized());
        }
        else if (dragging) {
            canvas->translateSelection(point - midPoint);
            midPoint = point;
        }
    }
}

void SelectTool::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && selecting) {
        if (event->pos() == origin) {
            canvas->showSelection(false);
        }
        selecting = false;
    }
    else if (dragging) {
        canvas->setCursor(oldCursor);
        dragging = false;
        if (event->button() == Qt::RightButton) { // reset if right clicked
            // TODO: we get a merge selection action... we don't want that
            canvas->translateSelection(origin - midPoint);
            canvas->showSelection(false);
        }
    }
}

QPoint SelectTool::boundPoint(QPoint pos)
{
    int x = pos.x();
    int y = pos.y();

    if (x < 0) {
        x = 0;
    }
    else if (x >= canvas->width()) {
        x = canvas->width()-1;
    }

    if (y < 0) {
        y = 0;
    }
    else if (y >= canvas->height()) {
        y = canvas->height()-1;
    }
    return QPoint(x,y);
}

