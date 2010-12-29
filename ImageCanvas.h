#ifndef IMAGECANVAS_H
#define IMAGECANVAS_H

#include <QColor>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QRubberBand>
#include "BackgroundLayer.h"
#include "ImageView.h"
#include "Layer.h"
#include "Tool.h"

class ImageCanvas : public ImageView
{
    Q_OBJECT
public:
    ImageCanvas(ImageDocument *document);
    void scaleImage(double factor);
    void resetScale();

    void setPenColor(const QColor &);
    void setPenWidth(int);
    void setSelectBox(QRect rect);
    void setSelectShow(bool show);
    void setTool(Tool::ToolTypes type);

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

    Tool	  * currentTool;
    QRubberBand   * rubberBand;
    int		    myPenWidth;
    QColor	    myPenColor;
};

#endif // IMAGECANVAS_H
