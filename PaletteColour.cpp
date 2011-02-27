#include "PaletteColour.h"

PaletteColour::PaletteColour(QColor colour_) :
        colour(colour_)
{
}

QRgb PaletteColour::getRGBA()
{
    return colour.rgba();
}
