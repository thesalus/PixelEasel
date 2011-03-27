#include "ImagePreview.h"
#include "Layers/SolidBackgroundLayer.h"
#include "ImageDocument.h"

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
    QImage  new_image = ((Layer) image.copy()).layOver(background->copy());
    int preview_width = 70;
    int preview_height = 70;
    int width = new_image.width();
    int height = new_image.height();
    if (width > preview_width) {
        if (height > width)
        {
            width = preview_width*width/height;
            height = preview_height;
        } else {
            height = preview_height*height/width;
            width = preview_width;
        }
    } else if (height > preview_height) {
        width = preview_width*width/height;
        height = preview_height;
    }
    this->setPixmap(QPixmap::fromImage(new_image.scaled(width, height)));
    this->repaint();
}
