#include "ImagePreview.h"
#include "SolidBackgroundLayer.h"
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
    this->setPixmap(QPixmap::fromImage(new_image));
    this->repaint();
}
