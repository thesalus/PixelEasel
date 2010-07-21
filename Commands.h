#ifndef COMMANDS_H
#define COMMANDS_H

#include <QUndoCommand>
#include <QImage>
#include "ImageDocument.h"

class AddCommand : public QUndoCommand
{
public:
    AddCommand(QImage old_image, QImage new_image, ImageDocument * document);
    void redo();
    void undo();
private:
    QImage oldImage;
    QImage newImage;
    ImageDocument *document;
};

#endif // COMMANDS_H
