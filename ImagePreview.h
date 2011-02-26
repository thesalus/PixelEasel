#ifndef IMAGEPREVIEW_H
#define IMAGEPREVIEW_H

#include "Tools/Tool.h"
#include "ImageView.h"
#include "Layers/BackgroundLayer.h"

class ImageDocument;

class ImagePreview : public ImageView
{
    Q_OBJECT
public:
    ImagePreview(ImageDocument *);
    ~ImagePreview();
    virtual void setTool(Tool::ToolTypes);

public slots:
    void refreshImage(const QImage&);

private:
    BackgroundLayer*background;

// resize the image upon resize
};

#endif // IMAGEPREVIEW_H
