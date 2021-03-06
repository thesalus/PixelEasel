#ifndef SELECTTOOL_H
#define SELECTTOOL_H

#include "Tool.h"
#include <QCursor>
#include <QMouseEvent>
class ImageCanvas;

class SelectTool : public Tool
{
public:
    SelectTool(ImageCanvas* canvas);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

private:
    ImageCanvas   * canvas;
    bool            dragging;
    bool            selecting;
    QCursor         oldCursor;
    QPoint	    origin;
    QPoint          midPoint;

    QPoint          boundPoint(QPoint);
};

#endif // SELECTTOOL_H
