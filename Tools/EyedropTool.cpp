#include "EyedropTool.h"
#include "ImageDocument.h"

EyedropTool::EyedropTool(ImageDocument* document)
    : document_m(document), sampling(false), colour(Qt::transparent)
{
}

void EyedropTool::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        colour = document_m->getPixel(event->pos());
        document_m->setColour(colour);
        sampling = true;
    }
    // RightButton to cancel the event?
}

void EyedropTool::mouseMoveEvent(QMouseEvent *event)
{
    if (sampling && (event->buttons() & Qt::LeftButton)) {
        colour = document_m->getPixel(event->pos());
        document_m->setColour(colour);
        // this shouldn't actually change hte value.
        document_m->refreshScratchpad();
    }
}

void EyedropTool::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && sampling) {
        document_m->setColour(colour);
        sampling = false;
    }
}
