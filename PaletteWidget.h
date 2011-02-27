#ifndef PALETTEWIDGET_H
#define PALETTEWIDGET_H

#include <QFrame>
#include <QVector>
#include "FlowLayout.h"
#include "Palette.h"
#include "PaletteColourWidget.h"

class PaletteWidget : public QFrame
{
    Q_OBJECT
public:
    explicit PaletteWidget(QWidget *parent = 0);
    ~PaletteWidget();

    void changeSwatch(Palette*);

private:
    Palette* palette;
    QVector<PaletteColourWidget*> swatch;
    FlowLayout *outer_layout;

signals:
    void selectedColour(PaletteColour*);

public slots:
    void addColour();
    void selected(PaletteColour*);
};

#endif // PALETTEWIDGET_H
