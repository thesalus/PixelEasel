#include "PaletteColourWidget.h"
#include <QColorDialog>

PaletteColourWidget::PaletteColourWidget(QWidget *parent, PaletteColour* colour_) :
    QLabel(parent),
    colour(colour_)
{
    setPaletteColour(colour);
}

void PaletteColourWidget::mousePressEvent(QMouseEvent *event)
{
    (void) event;
    emit selected(colour);
}

void PaletteColourWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    (void) event;
    // send a message back and change the indexed colour
    QColor new_colour = QColorDialog::getColor();
    colour = new PaletteColour(new_colour);
    setPaletteColour(colour);
    emit selected(colour);
}

void PaletteColourWidget::setPaletteColour(PaletteColour * colour)
{
    QImage image(QSize(size, size), QImage::Format_RGB32);
        image.fill(colour->getRGBA());
    this->setPixmap(QPixmap::fromImage(image));
    this->repaint();
}
