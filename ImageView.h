#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include "Tool.h"
#include <QLabel>

class ImageDocument;

class ImageView : public QLabel
{
    Q_OBJECT
public:
    ImageView(ImageDocument *document);
    virtual void setSelectBox(QRect rect);
    virtual void setTool(Tool::ToolTypes) = 0;  // do we want this to be a pure virtual method?

    virtual void showSelection(bool);
    virtual bool hasSelection();
    virtual QRect getSelection();
signals:

public slots:
    virtual void refreshImage(const QImage&) = 0;

protected:
    ImageDocument * document_m;
};

#endif // IMAGEVIEW_H
