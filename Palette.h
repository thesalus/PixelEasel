#ifndef PALETTE_H
#define PALETTE_H

#include <QRgb>
#include <QVector>
#include "PaletteColour.h"

class Palette
{
public:
    Palette();
    ~Palette();
    void addColour(QRgb);
    QVector<PaletteColour*>*    getColours();
    QVector<QRgb> getColourTable() const { return colourTable; }
    void swapColours(QRgb originalColour, QRgb nextColour);

private:
    QVector<PaletteColour*>     colours;
    QVector<QRgb> colourTable;
};

#endif // PALETTE_H
