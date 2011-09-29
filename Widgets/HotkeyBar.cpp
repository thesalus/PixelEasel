#include "HotkeyBar.h"
#include "Tools/Tool.h"
#include <QAction>
#include <QtAlgorithms>

//TODO: allow changes to Qt::ToolButtonStyle

HotkeyBar::HotkeyBar(QWidget *parent, unsigned int max_keys) :
    QToolBar("Hotkeys", parent),
    maxHotkeys(max_keys),
    selection(Tool::PenTool),
    tools(this),
    mapper(new QSignalMapper(this))
{
    createActions();
    connect(mapper, SIGNAL(mapped(int)),    // using an int here is an ugly hack
            parent, SLOT(setTool(int)));
    connect(mapper, SIGNAL(mapped(int)),
            this,   SLOT(setTool(int)));
    setTool(Tool::PenTool);
}

HotkeyBar::~HotkeyBar()
{
    qDeleteAll(tools.actions());
    delete mapper;
}

void HotkeyBar::createActions()
{
    createAction(QIcon("pencil.png"),   QString("Pencil"),
                 Qt::Key_P,             Tool::PenTool);
    createAction(QIcon("line.png"),     QString("Line"),
                 Qt::Key_L,             Tool::LineTool);
    createAction(QIcon("select.png"),   QString("Selection"),
                 Qt::Key_S,             Tool::SelectTool);
    createAction(QIcon("dropper.png"),  QString("Eye Drop"),
                 Qt::Key_E,             Tool::EyedropTool);
    tools.setExclusive(true);
}

void HotkeyBar::createAction(QIcon icon, QString text, QKeySequence shortcut, Tool::ToolTypes type)
{
    QAction *action = new QAction(&tools);
        action->setIcon(icon);
        action->setText(text);
        action->setToolTip(text);
        action->setShortcut(shortcut);
        action->setCheckable(true);
    mapper->setMapping(action, (int) type);
    addAction(action);
    connect(action, SIGNAL(triggered()),
            mapper, SLOT (map()));
}

void HotkeyBar::setTool(int new_selection)
{
    QAction *action;
    if ((action = (QAction*)mapper->mapping(new_selection)) != NULL) {
        action->setChecked(true);
        selection = (Tool::ToolTypes) new_selection;
    }
}

Tool::ToolTypes HotkeyBar::getToolType()
{
    return selection;
}

QList<QAction*> HotkeyBar::getActions()
{
    return tools.actions();
}
