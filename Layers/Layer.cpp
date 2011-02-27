#include "Layer.h"

Layer::Layer(QImage image)
    : QImage(image)
{
}

Layer::Layer(const QString & fileName, const char* format)
    : QImage(fileName, format)
{
}

Layer::Layer(const QSize & size, QImage::Format format)
    : QImage(size, format)
{
    this->fill(qRgba(0, 0, 0, 0));
}

QImage Layer::layOver (QImage bottomLayer)
{
    QPainter painter(&bottomLayer);
        painter.drawImage(QPoint(0,0),(*this));
        painter.end();

    return bottomLayer;
}
