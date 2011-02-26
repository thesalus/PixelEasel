#include "BackgroundLayer.h"

BackgroundLayer::BackgroundLayer(Pattern pattern, QSize size)
    :   Layer(size, QImage::Format_ARGB32_Premultiplied),
        pattern_m(pattern),
        size_m(size)
{
}
