#ifndef COMMANDS_H
#define COMMANDS_H

#include <QImage>
#include <QRgb>
#include <QUndoCommand>
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

class PaletteSwapCommand : public QUndoCommand
{
public:
    PaletteSwapCommand(QRgb original, QRgb replacement, ImageDocument * document);
    void redo();
    void undo();
private:
    QRgb oldColour;
    QRgb newColour;
    ImageDocument *document;
};

#endif // COMMANDS_H
