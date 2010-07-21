#ifndef IMAGEDOCUMENT_H
#define IMAGEDOCUMENT_H

#include <QMdiSubWindow>
#include <QImage>
#include <QScrollArea>
#include <QMessageBox>
#include "ImageCanvas.h"
#include <QUndoStack>
class ImageCanvas;

class ImageDocument : public QMdiSubWindow
{
    Q_OBJECT
public:
    ImageDocument(QString, QUndoStack*);
    ImageDocument(QSize, QUndoStack*);

    void scaleImage(double);
    void resetScale();
    void drawImage(QImage new_image);
    void drawLines(QPen pen, QVector<QPoint> pointPairs);
    void save(QString file = NULL);

    const QImage &  getImage();
    QString	    getPath();
    QSize	    getSize();
    void	    setSize(QSize newSize);

signals:
    void imageModified(const QImage &);

private:
    QString	    filename;
    QImage	    image;
    ImageCanvas*    canvas;
    QScrollArea*    scrollArea;
    QUndoStack*	    undoStack;

    void	    makeChange();
    void	    initialize();
};

#endif // IMAGEDOCUMENT_H
