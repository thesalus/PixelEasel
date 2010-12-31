#include "ImageView.h"

ImageView::ImageView(ImageDocument *document) :
    document_m(document)
{
}

bool ImageView::hasSelection()
{
    return false;
}

QRect ImageView::getSelection()
{
    return QRect();
}

void ImageView::showSelection(bool show)
{
}

void ImageView::setSelectBox(QRect rect)
{
}
