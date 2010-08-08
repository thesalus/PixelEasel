#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QLabel>

class ImageDocument;

class ImageView : public QLabel
{
    Q_OBJECT
public:
    ImageView(ImageDocument *document);

signals:

public slots:
    virtual void refreshImage(const QImage&) = 0;

protected:
    ImageDocument * document_m;
};

#endif // IMAGEVIEW_H
