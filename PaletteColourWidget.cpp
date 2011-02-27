#include "PaletteColourWidget.h"

PaletteColourWidget::PaletteColourWidget(QWidget *parent, PaletteColour* colour_) :
    QLabel(parent),
    colour(colour_)
{
    QSize size(25,25);
    QImage image(size, QImage::Format_RGB32);
        image.fill(colour->getRGBA());
    this->setPixmap(QPixmap::fromImage(image));
    this->repaint();
}

void PaletteColourWidget::mousePressEvent(QMouseEvent *event)
{
    emit selected(colour);
}
