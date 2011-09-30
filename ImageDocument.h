#ifndef IMAGEDOCUMENT_H
#define IMAGEDOCUMENT_H

#include <QClipboard>
#include <QHash>
#include <QImage>
#include <QMdiSubWindow>
#include <QMessageBox>
#include <QScrollArea>
#include <QUndoStack>
#include "ImageCanvas.h"
#include "ImagePreview.h"
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

    QImage*         getImage();
    Palette*        getPalette();
    QString         getPath();
    QRgb            getPixel(QPoint);
    QSize           getSize();
    QUndoStack*     getUndoStack();
    ImagePreview*   getPreview();
    int             getZoomInActiveView();

    void            setSelection(QRect rect);
    void            setSize(QSize new_size);
    void            setUndoStack(QUndoStack* new_undoStack);

    bool            hasFile();
    bool            hasSelection();
    void            setToolInActiveView(Tool::ToolTypes);
    void            setZoomInActiveView(int);
    void            setColour(PaletteColour*);
    void            setColour(QRgb colour);
    void            changeColour(PaletteColour*);
    void            changeColour(QRgb colour);
    void            translateSelection(QPoint point);
    void            selectToScratchpad();

    void            cut(QClipboard *clipboard);  // should we return something upon a successful action?
    void            copy(QClipboard *clipboard);
    void            paste(QClipboard *clipboard);

    void            clearSelection();

private slots:
    void updateTitle(bool);
    void passSelectionModified();

signals:
    void imageModified(const QImage &);
    bool selectionModified(bool);

private:
    static int	    c_untitled_documents;       // tracks the number that will be appended to new unnamed documents

    QString	    fileName;
    bool            isSavedToFile;
    bool            isSelectionChanged;
    bool            isScratchpadSelectionEmpty;

    int		    imageIndex;
    QVector<Layer*> imageLayers;
    Layer	    scratchpad;
    QVector<ImageView*>	views;
    QScrollArea    *scrollArea;
    QUndoStack     *undoStack;
    QImage          preScratch;
    QPoint          scratchpadTranslation;
    ImagePreview   *preview;
    Palette        *palette;
    QVector<QRgb>   colourTable;

    QPen            m_pen;
    int		    m_pen_width;
    QColor	    m_pen_colour;

    void            clearRect(QRect rect);
    void	    makeChange();   // make change, and refresh image
    void	    initialize();
    void            initializeColourTable();
    void            addColourToTable(QRgb key, QHash<QRgb, int> & colourHash);
};

#endif // IMAGEDOCUMENT_H
