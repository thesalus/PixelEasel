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

private:
    void createUndoView();
    void createActions();
    void createHotkeys();
    void createMenus();
    void createDocks();
    void adjustScrollBar(QScrollBar *scrollBar, double factor);
    void setupContext(ImageDocument*);
    ImageDocument *activeDocument();
    void closeEvent(QCloseEvent* e);

    QScrollArea     *scrollArea;
    QMdiArea        *mdiArea;
    QUndoView       *undoView;
    QClipboard      *clipboard;

    QDockWidget     *dock;
    QDockWidget     *previewDock;
    QStackedWidget  *previews;
    QDockWidget     *historyDock;
    QUndoGroup      *undoGroup;

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
