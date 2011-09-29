#include "Commands.h"

AddCommand::AddCommand(QImage old_image, QImage new_image, ImageDocument * document_)
    : oldImage(old_image)
    , newImage(new_image)
    , document(document_)
{
}

void AddCommand::redo()
{
    this->document->replaceImage(this->newImage);
}

void AddCommand::undo()
{
    this->document->replaceImage(this->oldImage);
}
