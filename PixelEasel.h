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
#include "Widgets/HotkeyBar.h"
#include "Widgets/ZoomWidget.h"
#include "Widgets/PaletteWidget.h"
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
    void clearSelection();
    void selectAll();
    void zoomIn();
    void zoomOut();
    void normalSize();
    void resizeImage();
    void about();

    void editActions();
    void saveActions();

    void updateActions();
    void updateEditActions(bool);
    void updateContext(QMdiSubWindow*);
    void updateChangedColour(PaletteColour*);
    void updateSave(bool);

    void setTool(int);
    void setColour(PaletteColour*);

    void zoomChanged(int);

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

    void loadActions();

    QMdiArea        *mdiArea;
    QUndoView       *undoView;
    QClipboard      *clipboard;

    QStackedWidget  *previews;
    QDockWidget     *historyDock;
    QDockWidget     *paletteDock;
    QDockWidget     *previewDock;
    QUndoGroup      *undoGroup;
    QDockWidget     *viewDock;
    ZoomWidget      *zoomWidget;

    QMdiSubWindow   *oldWindow;
    PaletteWidget   *paletteView;
    HotkeyBar       *hotkeys;

    QAction *newAction;
    QAction *openAction;
    QAction *saveAction;
    QAction *saveAsAction;
    QAction *exitAction;

    QAction *copyAction;
    QAction *cutAction;
    QAction *pasteAction;

    QAction *clearSelectionAction;
    QAction *selectAllAction;

    QAction *zoomInAction;
    QAction *zoomOutAction;
    QAction *normalSizeAction;

    QAction *undoAction;
    QAction *redoAction;

    QAction *resizeAction;

    QAction *aboutAction;
    QAction *aboutQtAction;

    QAction *editActionsAction;
    QAction *saveActionsAction;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *viewMenu;
    QMenu *imageMenu;
    QMenu *helpMenu;
};

#endif // PIXELEASEL_H
