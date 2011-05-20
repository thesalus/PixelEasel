#ifndef IMAGECANVAS_H
#define IMAGECANVAS_H

#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QRubberBand>
#include "Layers/BackgroundLayer.h"
#include "ImageView.h"
#include "Layers/Layer.h"
#include "Tools/Tool.h"

class ImageCanvas : public ImageView
{
    Q_OBJECT
public:
    static int default_scale_factor;
    static QString  scale_factor_strings[];

    ImageCanvas(ImageDocument *document);
    ~ImageCanvas();
    void resetScale();

    void setSelectBox(QRect rect);
    void setTool(Tool::ToolTypes type);

    bool hasSelection();
    void showSelection(bool show);
    QRect getSelection();
    void translateSelection(QPoint);
    void setScale(int factor);

    int  getScale();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

public slots:
    void refreshImage(const QImage&);

signals:
    void selectionModified();

private:
    QPoint normalizePoint(QPoint point);
    void resizeImage(QImage *image, const QSize &newSize);

    BackgroundLayer*background;

    static double   scale_factors[];
    int             m_scale;

    Tool	   *currentTool;
    QRubberBand    *rubberBand;
    QRect           mySelection;
};

#endif // IMAGECANVAS_H
