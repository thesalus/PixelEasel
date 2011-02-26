#ifndef BACKGROUNDLAYER_H
#define BACKGROUNDLAYER_H

#include <QColor>
#include "Layer.h"

class BackgroundLayer : public Layer
{
public:
    enum Pattern
    {
	Pattern_Solid,
	Pattern_SquareCheckered,
	Pattern_IsometricCheckered,
	Pattern_TiledImage,
	Pattern_Image
    };

    BackgroundLayer(Pattern pattern, QSize size);
    virtual BackgroundLayer* copySized(QSize size) = 0;

protected:
    Pattern	    pattern_m;
    QColor	    primaryColour_m,
		    secondaryColour_m;
    QSize           size_m;

};

#endif // BACKGROUNDLAYER_H
