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
    void swapColours(QRgb originalColour, QRgb nextColour);

    QVector<PaletteColour*>* getColours()  { return &colours; }
    QVector<QRgb> getColourTable() const { return colourTable; }

    void setActiveColour(QRgb);
    QRgb getActiveColour();

private:
    QVector<PaletteColour*> colours;
    QVector<QRgb> colourTable;

    int activeIndex;
};

#endif // PALETTE_H
