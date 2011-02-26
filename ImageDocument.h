#ifndef IMAGEDOCUMENT_H
#define IMAGEDOCUMENT_H

#include <QMdiSubWindow>
#include <QImage>
#include <QScrollArea>
#include <QClipboard>
#include <QMessageBox>
#include "ImageCanvas.h"
#include "ImagePreview.h"
#include <QUndoStack>
#include "Layers/Layer.h"
#include "Palette.h"
#include "PaletteColour.h"
class ImageCanvas;

class ImageDocument : public QMdiSubWindow
{
    Q_OBJECT
public:
    ImageDocument(QString);
    ImageDocument(QSize);
    ~ImageDocument();

    void scaleImage(double scale_factor);
    void resetScale();
// Permanent Actions
    void replaceImage(QImage new_image);
    void drawImage(QImage new_image);
    void drawLines(QVector<QPoint> point_pairs);
// Temporary Actions
    void refreshScratchpad();
    void scratchLine(QPoint start_point, QPoint end_point);

    void save();
    void setFileName(QString file);

    void closeEvent(QCloseEvent* event);

    QImage*	    getImage();
    Palette*        getPalette();
    QString	    getPath();
    QSize	    getSize();
    QUndoStack*     getUndoStack();
    ImagePreview*   getPreview();

    void            setSelection(QRect rect);
    void	    setSize(QSize new_size);
    void	    setUndoStack(QUndoStack* new_undo_stack);

    bool            hasFile();
    bool            hasSelection();
    void            setToolInActiveView(Tool::ToolTypes);
    void            setColour(PaletteColour*);
    void            translateSelection(QPoint point);
    void            selectToScratchpad();

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
    static int	    untitled_counter;       // tracks the number that will be appended to new unnamed documents
    QString	    file_name;
    bool            has_file_flag;
    bool            selection_changed_flag;
    bool            emptyScratchpadSelection;
    int		    image_index;
    QVector<Layer*> image_layers;
    Layer	    scratchpad;
    QVector<ImageView*>	views;
    QScrollArea*    scroll_area;
    QUndoStack*	    undo_stack;
    QImage          preScratch;
    QPoint          scratchpad_translation;
    ImagePreview*   preview;
    Palette*        palette;

    QPen            myPen;
    int		    myPenWidth;
    QColor	    myPenColor;

    void            clearRect(QRect rect);
    void	    makeChange();
		    // make change, and refresh image
    void	    initialize();
};

#endif // IMAGEDOCUMENT_H
