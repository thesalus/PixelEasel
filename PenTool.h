#ifndef PENTOOL_H
#define PENTOOL_H

#include "Tool.h"
#include <QMouseEvent>
#include <QVector>

#include <QPen>
class ImageCanvas;

class PenTool : public Tool
{
public:
    PenTool(ImageCanvas* canvas, QPen pen_);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
private:

    ImageCanvas *imageCanvas;
    QVector<QPoint> pointPairs;
    bool scribbling;
    QPoint lastPoint;
    QPen pen;
};

#endif // PENTOOL_H
