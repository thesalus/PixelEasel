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

void Layer::swapColours(QRgb oldColour, QRgb newColour)
{
    for (int x = 0; x < width(); x++) {
        for (int y = 0; y < height(); y++) {
            if (this->pixel(x,y) == oldColour) {
                this->setPixel(x, y, newColour);
            }
        }
    }
}
