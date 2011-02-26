#include "Palette.h"

Palette::Palette()
{
}

QVector<PaletteColour*>* Palette::getColours()
{
    return &colours;
}

void Palette::addColour(QRgb colour)
{
    colours.push_back(new PaletteColour(colour));
}
