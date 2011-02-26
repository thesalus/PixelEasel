#ifndef HOTKEYBAR_H
#define HOTKEYBAR_H

#include "Tool.h"

#include <QSignalMapper>
#include <QToolBar>
#include <QToolButton>

class HotkeyBar : public QToolBar
{
    Q_OBJECT
    unsigned int max_hotkeys;
    Tool::ToolTypes selection;
    QToolButton** buttons;   // do we need to customize these too? drag+drop...
    QSignalMapper *mapper;
    void createButtons();
public:
    explicit HotkeyBar(QWidget *parent = 0, unsigned int max_keys = 2);
    Tool::ToolTypes getToolType();

signals:

public slots:
    void setTool(int new_selection);
};

#endif // HOTKEYBAR_H
