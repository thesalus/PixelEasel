#ifndef PALETTE_H
#define PALETTE_H

#include <QRgb>
#include <QVector>
#include "PaletteColour.h"

class Palette
{
public:
    Palette();
    void addColour(QRgb);
    QVector<PaletteColour*>*    getColours();

private:
    QVector<PaletteColour*>     colours;
};

#endif // PALETTE_H
