#ifndef SOLIDBACKGROUNDLAYER_H
#define SOLIDBACKGROUNDLAYER_H

#include <QColor>
#include "BackgroundLayer.h"

class SolidBackgroundLayer : public BackgroundLayer
{
public:
    SolidBackgroundLayer(QSize size, QColor colour);
    BackgroundLayer* copySized(QSize size);
};

#endif // SOLIDBACKGROUNDLAYER_H
