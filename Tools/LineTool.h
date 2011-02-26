#ifndef LINETOOL_H
#define LINETOOL_H

#include "Tool.h"
#include <QMouseEvent>
#include <QVector>

#include <QPen>
class ImageDocument;

class LineTool : public Tool
{
public:
    LineTool(ImageDocument* document);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
private:

    ImageDocument * document_m;
    bool	    scribbling;
    QPoint	    curPoint;
    QPoint	    lastPoint;
};

#endif // LINETOOL_H
