#ifndef EYEDROPTOOL_H
#define EYEDROPTOOL_H

#include "Tool.h"
#include <QMouseEvent>
#include <QRgb>
class ImageDocument;

class EyedropTool : public Tool
{
public:
    EyedropTool(ImageDocument*);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

private:
    ImageDocument * document_m;
    bool            sampling;
    QRgb            colour;
};

#endif // EYEDROPTOOL_H
