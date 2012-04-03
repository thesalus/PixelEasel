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
    if (colour != Qt::transparent && colourTable.indexOf(colour) == -1) {
        colourTable.push_back(colour);
        colours.push_back(new PaletteColour(colour));
    }
}

void Palette::swapColours(QRgb originalColour, QRgb newColour)
{
    // TODO: update the palette widget
    int index = colourTable.indexOf(originalColour);
    if (index == -1) {
        addColour(newColour);
    } else {
        colourTable[index] = newColour;
        colours[index] = new PaletteColour(newColour);
    }
}
