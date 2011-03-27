#include "PaletteColourWidget.h"
#include <QColorDialog>

PaletteColourWidget::PaletteColourWidget(QWidget *parent, PaletteColour* colour_) :
    QLabel(parent),
    colour(colour_)
{
    QImage image(QSize(25, 25), QImage::Format_RGB32);
        image.fill(colour->getRGBA());
    this->setPixmap(QPixmap::fromImage(image));
    this->repaint();
}

void PaletteColourWidget::mousePressEvent(QMouseEvent *event)
{
    emit selected(colour);
}


void PaletteColourWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    // send a message back and change the indexed colour
    QColor new_colour = QColorDialog::getColor();
    colour = new PaletteColour(new_colour);
    QImage image(QSize(25, 25), QImage::Format_RGB32);
        image.fill(colour->getRGBA());
    this->setPixmap(QPixmap::fromImage(image));
    this->repaint();

    emit selected(colour);
}
