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
    emit selected(this);
}

void PaletteColourWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    (void) event;
    // send a message back and change the indexed colour
    QColor newColour = QColorDialog::getColor(colour->getRGBA());
    if (newColour.isValid()) {
        colour = new PaletteColour(newColour);
        setPaletteColour(colour);
        emit selected(this);
    }
}

void PaletteColourWidget::setPaletteColour(PaletteColour * colour)
{
    this->colour = colour;
    QImage image(QSize(size, size), QImage::Format_RGB32);
        image.fill(colour->getRGBA());
    this->setPixmap(QPixmap::fromImage(image));
    this->repaint();
}

PaletteColour * PaletteColourWidget::getPaletteColour()
{
    return this->colour;
}
