#include "ImagePreview.h"
#include "Layers/SolidBackgroundLayer.h"
#include "ImageDocument.h"

ImagePreview::ImagePreview(ImageDocument *document)
    :   ImageView(document),
        background(new SolidBackgroundLayer(document->getSize(), Qt::green))
{
    this->setAlignment(Qt::AlignCenter);
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
    int width = new_image.width();
    int height = new_image.height();
    if (width > 70) {
        if (height > width)
        {
            width = 70*width/height;
            height = 70;
        } else {
            height = 70*height/width;
            width = 70;
        }
    } else if (height > 70) {
        width = 70*width/height;
        height = 70;
    }
    this->setPixmap(QPixmap::fromImage(new_image.scaled(width, height)));
    this->repaint();
}
