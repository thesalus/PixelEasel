#include "SolidBackgroundLayer.h"

SolidBackgroundLayer::SolidBackgroundLayer(QSize size, QColor colour)
    : BackgroundLayer(BackgroundLayer::Pattern_Solid, size)
{
    this->primaryColour_m = colour;
    QPainter painter(this);
    painter.fillRect(0,0,size.width(),size.height(),colour);
}

BackgroundLayer* SolidBackgroundLayer::copySized(QSize size)
{
    return new SolidBackgroundLayer(size, this->primaryColour_m);

}
