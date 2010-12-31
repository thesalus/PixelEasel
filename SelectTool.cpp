#include "SelectTool.h"
#include "ImageDocument.h"

SelectTool::SelectTool(ImageCanvas* canvas_)
    : canvas(canvas_), selecting(false)
{
}

void SelectTool::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        origin = event->pos();
        canvas->setSelectBox(QRect(origin, QSize(1, 1)));
        selecting = true;
    }
}

void SelectTool::mouseMoveEvent(QMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton) && selecting)
    {
        int x = event->pos().x();
        if (x < 0)
        {
            x = 0;
        }
        else if (x >= canvas->width())
        {
            x = canvas->width()-1;
        }
        int y = event->pos().y();
        if (y < 0)
        {
            y = 0;
        }
        else if (y >= canvas->height())
        {
            y = canvas->height()-1;
        }
        canvas->setSelectBox(QRect(origin, QPoint(x, y)).normalized());
    }
}

void SelectTool::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && selecting)
    {
        if (event->pos() == origin)
        {
            canvas->showSelection(false);
        }
        selecting = false;
    }
}
