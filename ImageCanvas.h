#ifndef IMAGECANVAS_H
#define IMAGECANVAS_H

#include <QPainter>
#include <QLabel>
#include <QColor>
#include <QMouseEvent>
#include "Tool.h"
#include "PenTool.h"
#include "Layer.h"
#include "BackgroundLayer.h"

class ImageDocument;

class ImageCanvas : public QLabel
{
    Q_OBJECT
public:
    ImageCanvas(ImageDocument *document);
    void scaleImage(double factor);
    void resetScale();
    void setPenColor(const QColor &);
    void setPenWidth(int);
    QPoint getPoint(QMouseEvent *event);

    void drawLine(const QPoint &startPoint, const QPoint &endPoint, const QPen &pen);

    void commitLines(const QVector<QPoint> &pointPairs, const QPen &pen);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

public slots:
    void refreshImage(const QImage&);

private:
    void setScale(double factor);
    void refreshScratchpad();
    void resizeImage(QImage *image, const QSize &newSize);

    ImageDocument * document_m;
    BackgroundLayer*background;
    Layer	    scratchpad;

    double	    scaleFactor;

    Tool*	    currentTool;
    int		    myPenWidth;
    QColor	    myPenColor;
};

#endif // IMAGECANVAS_H
