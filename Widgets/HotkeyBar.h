#ifndef HOTKEYBAR_H
#define HOTKEYBAR_H

#include "Tools/Tool.h"

#include <QSignalMapper>
#include <QActionGroup>
#include <QToolBar>

class HotkeyBar : public QToolBar
{
    Q_OBJECT
    unsigned int    maxHotkeys;
    Tool::ToolTypes selection;
    QActionGroup    tools;   // do we need to customize these too? drag+drop...
    QSignalMapper  *mapper;

    void createActions();
    void createAction(QIcon icon, QString text, QKeySequence shortcut, Tool::ToolTypes type);
public:
    explicit HotkeyBar(QWidget *parent = 0, unsigned int max_keys = 2);
    ~HotkeyBar();
    Tool::ToolTypes getToolType();
    QList<QAction*> getActions();

signals:

public slots:
    void setTool(int new_selection);
};

#endif // HOTKEYBAR_H
