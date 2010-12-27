#ifndef PIXELEASEL_H
#define PIXELEASEL_H

#include <QMainWindow>
#include <QApplication>
#include <QMenuBar>
#include <QMenu>
#include <QAction>

#include <QScrollBar>
#include <QFileDialog>
#include <QDir>
#include <QMdiArea>
#include <QUndoGroup>
#include <QUndoStack>
#include <QDockWidget>
#include <QUndoView>
#include "ImageDocument.h"

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
    void zoomIn();
    void zoomOut();
    void normalSize();
    void resizeImage();
    void about();

    void updateActions();
    void updateContext(QMdiSubWindow*);
    void updateSave(bool);

private:
    void createUndoView();
    void createActions();
    void createMenus();
    void adjustScrollBar(QScrollBar *scrollBar, double factor);
    void setupContext(ImageDocument*);
    ImageDocument *activeDocument();
    int exit();

    QScrollArea *scrollArea;
    QMdiArea *mdiArea;
    QDockWidget *dock;
    QUndoGroup *undoGroup;
    QUndoView *undoView;

    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *exitAct;

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
