#ifndef PALETTECOLOURWIDGET_H
#define PALETTECOLOURWIDGET_H

#include <QLabel>
#include "PaletteColour.h"

class PaletteColourWidget : public QLabel
{
    Q_OBJECT
public:
    static const int size = 25;

    explicit PaletteColourWidget(QWidget *parent, PaletteColour* colour);
    void setPaletteColour(PaletteColour * colour);

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseDoubleClickEvent(QMouseEvent *event);
    //void mouseMoveEvent(QMouseEvent *event);
    //void mouseReleaseEvent(QMouseEvent *event);

private:
    PaletteColour* colour;

signals:
    void selected(PaletteColour*);

public slots:

};

#endif // PALETTECOLOURWIDGET_H
