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
    ~ImageDocument();

    void scaleImage(double);
    void resetScale();
// Permanent Actions
    void replaceImage(QImage new_image);
    void drawImage(QImage new_image);
    void drawLines(QPen pen, QVector<QPoint> pointPairs);
// Temporary Actions
    void refreshScratchpad();
    void scratchLine(QPen pen, QPoint startPoint, QPoint endPoint);

    void save();
    void setFileName(QString file);

    QImage *	    getImage();
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
    static int	    untitled_counter;
    QString	    fileName;
    int		    imageIndex;
    QVector<Layer*> imageLayers;
    Layer	    scratchpad;
    QVector<ImageView*>	views;
    QScrollArea*    scrollArea;
    QUndoStack*	    undoStack;

    void	    makeChange();
		    // make change, and refresh image
    void	    initialize();
};

#endif // IMAGEDOCUMENT_H
