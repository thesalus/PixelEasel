#include "ImagePreview.h"
#include "Layers/SolidBackgroundLayer.h"
#include "ImageDocument.h"

#define PREVIEW_SIZE 70

ImagePreview::ImagePreview(ImageDocument *document)
    :   ImageView(document),
        background(new SolidBackgroundLayer(document->getSize(), Qt::green))
{
    this->setAlignment(Qt::AlignCenter);

    // there should be different types of previews
    // it should resize to the size of the box
    // or it should be in a scrollable area.
}

ImagePreview::~ImagePreview()
{
    delete background;
}

void ImagePreview::setTool(Tool::ToolTypes)
{
}

void ImagePreview::refreshImage(const QImage& image)
{
    QImage  newImage  = ((Layer) image.copy()).layOver(background->copy());
    int previewWidth  = PREVIEW_SIZE;
    int previewHeight = PREVIEW_SIZE;
    int width  = newImage.width();
    int height = newImage.height();
    if (width > previewWidth) {
        if (height > width) {
            width = previewWidth*width/height;
            height = previewHeight;
        }
        else {
            height = previewHeight*height/width;
            width = previewWidth;
        }
    } else if (height > previewHeight) {
        width = previewWidth*width/height;
        height = previewHeight;
    }
    this->setPixmap(QPixmap::fromImage(newImage.scaled(width, height)));
    this->repaint();
}
