#ifndef PALETTECOLOUR_H
#define PALETTECOLOUR_H

#include <QColor>
#include <QRgb>

class PaletteColour
{
public:
    PaletteColour(QColor);
    QRgb getRGBA();
private:
    QColor colour;
};

#endif // PALETTECOLOUR_H
