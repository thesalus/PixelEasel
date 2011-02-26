#include "HotkeyBar.h"
#include "Tools/Tool.h"

//TODO: allow changes to Qt::ToolButtonStyle

HotkeyBar::HotkeyBar(QWidget *parent, unsigned int max_keys) :
    QToolBar("Hotkeys", parent),
    max_hotkeys(max_keys),
    selection(Tool::PenTool),
    buttons(new QToolButton*[10]),
    mapper(new QSignalMapper(this))
{
    createButtons();
    // TODO: maintain selection information somewhere (off to the side?) or merely show it as selected

    for (unsigned int i = 0; i < max_hotkeys; i++)
    {
        addWidget(buttons[i]);
        connect(buttons[i], SIGNAL(clicked()),
                mapper, SLOT (map()));
    }
    connect(mapper, SIGNAL(mapped(int)),    // using an int here is an ugly hack
            parent, SLOT(setTool(int)));
    connect(mapper, SIGNAL(mapped(int)),
            this, SLOT(setTool(int)));
    setTool(Tool::PenTool);
}

void HotkeyBar::createButtons()
{
    // need to look at how to let the user specify their own hotkeys in the toolbar
    buttons[0] = new QToolButton;
    buttons[0]->setIcon(QIcon("pencil.png"));
    buttons[0]->setText(QString("Pencil Tool"));
    mapper->setMapping(buttons[0], (int) Tool::PenTool);

    buttons[1] = new QToolButton;
    buttons[1]->setIcon(QIcon("line.png"));
    buttons[1]->setText(QString("Line Tool"));
    mapper->setMapping(buttons[1], (int) Tool::LineTool);

    buttons[2] = new QToolButton;
    buttons[2]->setIcon(QIcon("select.png"));
    buttons[2]->setText(QString("Selection"));
    mapper->setMapping(buttons[2], (int) Tool::SelectTool);
}

void HotkeyBar::setTool(int new_selection)
{
    QToolButton *foo;
    if ((foo = (QToolButton*)mapper->mapping((int) selection)) != NULL)
    {
        foo->setDown(false);
    }
    if ((foo = (QToolButton*)mapper->mapping(new_selection)) != NULL)
    {
        foo->setDown(true);
    }
    selection = (Tool::ToolTypes) new_selection;
}

Tool::ToolTypes HotkeyBar::getToolType()
{
    return selection;
}
