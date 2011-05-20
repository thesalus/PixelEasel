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

    QMdiArea        *mdi_area;
    QUndoView       *undo_view;
    QClipboard      *clipboard;

    QStackedWidget  *previews;
    QDockWidget     *history_dock;
    QDockWidget     *palette_dock;
    QDockWidget     *preview_dock;
    QUndoGroup      *undo_group;
    QDockWidget     *view_dock;
    ZoomWidget      *zoom_widget;

    QMdiSubWindow   *old_window;
    PaletteWidget   *palette_view;
    HotkeyBar       *hotkeys;

    QAction *new_action;
    QAction *open_action;
    QAction *save_action;
    QAction *save_as_action;
    QAction *exit_action;

    QAction *copy_action;
    QAction *cut_action;
    QAction *paste_action;

    QAction *clear_selection_action;
    QAction *select_all_action;

    QAction *zoom_in_action;
    QAction *zoom_out_action;
    QAction *normal_size_action;

    QAction *undo_action;
    QAction *redo_action;

    QAction *resize_action;

    QAction *about_action;
    QAction *about_qt_action;

    QAction *edit_actions_action;
    QAction *save_actions_action;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *viewMenu;
    QMenu *imageMenu;
    QMenu *helpMenu;
};

#endif // PIXELEASEL_H
