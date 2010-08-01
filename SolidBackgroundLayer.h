#ifndef SOLIDBACKGROUNDLAYER_H
#define SOLIDBACKGROUNDLAYER_H

#include <QColor>
#include "BackgroundLayer.h"

class SolidBackgroundLayer : public BackgroundLayer
{
public:
    SolidBackgroundLayer(QSize size, QColor colour);
    QSize size_m;
};

#endif // SOLIDBACKGROUNDLAYER_H
