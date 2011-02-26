#include "PaletteColour.h"

PaletteColour::PaletteColour(QColor colour_) :
        colour(colour_)
{
}

QRgb PaletteColour::getRGB()
{
    return colour.rgb();
}
