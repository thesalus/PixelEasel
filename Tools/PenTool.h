#ifndef PENTOOL_H
#define PENTOOL_H

#include "Tool.h"
#include <QMouseEvent>
#include <QVector>

#include <QPen>
class ImageDocument;

class PenTool : public Tool
{
public:
    PenTool(ImageDocument* document);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
private:

    ImageDocument * document_m;
    QVector<QPoint> pointPairs;
    bool	    scribbling;
    QPoint	    lastPoint;
};

#endif // PENTOOL_H
