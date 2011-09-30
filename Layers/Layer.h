#ifndef LAYER_H
#define LAYER_H

#include <QImage>
#include <QPainter>

class Layer : public QImage
{
public:
    Layer(QImage image);
    Layer (const QString & fileName, const char* format = 0);
    Layer (const QSize & size, QImage::Format format);

    QImage layOver (QImage bottomLayer);
    void swapColours(QRgb oldColour, QRgb newColour);
};

#endif // LAYER_H
