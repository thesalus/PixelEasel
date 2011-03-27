#include "Palette.h"
#include <QtAlgorithms>

Palette::Palette()
{
}

Palette::~Palette()
{
    qDeleteAll(colours);
}

QVector<PaletteColour*>* Palette::getColours()
{
    return &colours;
}

void Palette::addColour(QRgb colour)
{
    colours.push_back(new PaletteColour(colour));
}
