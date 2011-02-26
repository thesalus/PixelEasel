#ifndef TOOL_H
#define TOOL_H

#include <QMouseEvent>

// use the state design pattern for tools. makes sense


class Tool
{
public:
    Tool();
    enum ToolTypes { LineTool, PenTool, SelectTool };
    virtual void mousePressEvent(QMouseEvent *event) = 0;
    virtual void mouseMoveEvent(QMouseEvent *event) = 0;
    virtual void mouseReleaseEvent(QMouseEvent *event) = 0;
};

#endif // TOOL_H
