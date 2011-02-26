#ifndef PIXELEASEL_H
#define PIXELEASEL_H

#include <QMainWindow>
#include <QApplication>
#include <QMenuBar>
#include <QMenu>
#include <QAction>

#include <QStackedWidget>
#include <QScrollBar>
#include <QMdiArea>
#include <QClipboard>
#include <QUndoGroup>
#include <QUndoStack>
#include <QDockWidget>
#include <QToolBox>
#include <QUndoView>
#include "ImageDocument.h"
#include "HotkeyBar.h"
#include "PaletteWidget.h"
#include "PaletteColour.h"
#include <QDockWidget>

class PixelEasel : public QMainWindow {
    Q_OBJECT
public:
    PixelEasel(QWidget *parent = 0);
    ~PixelEasel();

private slots:
    void newFile();
    void open();
    void save();
    void saveAs();
    void copy();
    void cut();
    void paste();
    void selectAll();
    void zoomIn();
    void zoomOut();
    void normalSize();
    void resizeImage();
    void about();

    void updateActions();
    void updateEditActions(bool);
    void updateContext(QMdiSubWindow*);
    void updateSave(bool);

    void setTool(int);
    void setColour(PaletteColour*);

private:
    void createUndoView();
    void createActions();
    void createHotkeys();
    void createMenus();
    void createDocks();
    void adjustScrollBar(QScrollBar *scroll_bar, double factor);
    void setupContext(ImageDocument*);
    ImageDocument *activeDocument();
    void closeEvent(QCloseEvent* e);

    QMdiArea        *mdi_area;
    QUndoView       *undo_view;
    QClipboard      *clipboard;

    QStackedWidget  *previews;
    QDockWidget     *history_dock;
    QDockWidget     *palette_dock;
    QDockWidget     *preview_dock;
    QUndoGroup      *undo_group;

    QMdiSubWindow   *old_window;
    PaletteWidget   *palette_view;
    HotkeyBar       *hotkeys;

    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *exitAct;

    QAction *copyAct;
    QAction *cutAct;
    QAction *pasteAct;

    QAction *selectAllAct;

    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *normalSizeAct;

    QAction *undoAct;
    QAction *redoAct;

    QAction *resizeAct;

    QAction *aboutAct;
    QAction *aboutQtAct;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *viewMenu;
    QMenu *imageMenu;
    QMenu *helpMenu;
};

#endif // PIXELEASEL_H
