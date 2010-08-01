#ifndef IMAGEDOCUMENT_H
#define IMAGEDOCUMENT_H

#include <QMdiSubWindow>
#include <QImage>
#include <QScrollArea>
#include <QMessageBox>
#include "ImageCanvas.h"
#include <QUndoStack>
#include "Layer.h"
class ImageCanvas;

class ImageDocument : public QMdiSubWindow
{
    Q_OBJECT
public:
    ImageDocument(QString);
    ImageDocument(QSize);

    void scaleImage(double);
    void resetScale();
    void replaceImage(QImage new_image);
    void drawImage(QImage new_image);
    void drawLines(QPen pen, QVector<QPoint> pointPairs);
    void save(QString file = NULL);

    const QImage &  getImage();
    QString	    getPath();
    QSize	    getSize();
    QUndoStack *    getUndoStack();

    void	    setSize(QSize newSize);
    void	    setUndoStack(QUndoStack* undoStack_);

private slots:
    void updateTitle(bool);

signals:
    void imageModified(const QImage &);

private:
    QString	    fileName;
    Layer	    image;
    ImageCanvas*    canvas;
    QScrollArea*    scrollArea;
    QUndoStack*	    undoStack;

    void	    makeChange();
    void	    initialize();
};

#endif // IMAGEDOCUMENT_H
