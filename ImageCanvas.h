#ifndef IMAGECANVAS_H
#define IMAGECANVAS_H

#include <QPainter>
#include <QLabel>
#include <QColor>
#include <QMouseEvent>
#include "Tool.h"
#include "Layer.h"
#include "BackgroundLayer.h"
#include "ImageView.h"

class ImageCanvas : public ImageView
{
    Q_OBJECT
public:
    ImageCanvas(ImageDocument *document);
    void scaleImage(double factor);
    void resetScale();
    void setPenColor(const QColor &);
    void setPenWidth(int);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

public slots:
    void refreshImage(const QImage&);

private:
    void setScale(double factor);
    QPoint normalizePoint(QPoint point);
    void resizeImage(QImage *image, const QSize &newSize);

    BackgroundLayer*background;

    double	    scaleFactor;

    Tool*	    currentTool;
    int		    myPenWidth;
    QColor	    myPenColor;
};

#endif // IMAGECANVAS_H
