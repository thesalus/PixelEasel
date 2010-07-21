#include "Commands.h"

AddCommand::AddCommand(QImage old_image, QImage new_image, ImageDocument * document_)
    : oldImage(old_image), newImage(new_image), document(document_)
{
}

void AddCommand::redo()
{
    document->drawImage(newImage);
}

void AddCommand::undo()
{
    document->drawImage(oldImage);
}
