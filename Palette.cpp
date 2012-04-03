#include "Palette.h"
#include <QtAlgorithms>

Palette::Palette()
    : activeIndex(0)
{
}

Palette::~Palette()
{
    qDeleteAll(colours);
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
    // TODO: update the palette widget. Perhaps make this a distinct entity
    //       and wire it with signals.
    int index = colourTable.indexOf(originalColour);
    if (index == -1) {
        addColour(newColour);
    } else {
        colourTable[index] = newColour;
        colours[index] = new PaletteColour(newColour);
    }
}

void Palette::setActiveColour(QRgb colour)
{
    int index = colourTable.indexOf(colour);
    if (index == -1) {
        addColour(colour);
        index = colourTable.indexOf(colour);
    }
    activeIndex = index;
}

QRgb Palette::getActiveColour() {
    return colourTable[activeIndex];
}
