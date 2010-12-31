#ifndef IMAGEDOCUMENT_H
#define IMAGEDOCUMENT_H

#include <QMdiSubWindow>
#include <QImage>
#include <QScrollArea>
#include <QClipboard>
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

    void closeEvent(QCloseEvent* event);

    QImage *	    getImage();
    QString	    getPath();
    QSize	    getSize();
    QUndoStack *    getUndoStack();

    void	    setSize(QSize newSize);
    void	    setUndoStack(QUndoStack* undoStack_);

    bool            hasFile();
    bool            hasSelection();
    void            setToolInActiveView(Tool::ToolTypes);

    void            cut(QClipboard *clipboard);  // should we return something upon a successful action?
    void            copy(QClipboard *clipboard);
    void            paste(QClipboard *clipboard);

private slots:
    void updateTitle(bool);
    void passSelectionModified();

signals:
    void imageModified(const QImage &);
    bool selectionModified(bool);

private:
    static int	    untitled_counter;
    QString	    fileName;
    bool            hasFileFlag;
    int		    imageIndex;
    QVector<Layer*> imageLayers;
    Layer	    scratchpad;
    QVector<ImageView*>	views;
    QScrollArea*    scrollArea;
    QUndoStack*	    undoStack;

    void            clearRect(QRect rect);
    void	    makeChange();
		    // make change, and refresh image
    void	    initialize();
};

#endif // IMAGEDOCUMENT_H
