#ifndef PALETTEWIDGET_H
#define PALETTEWIDGET_H

#include <QFrame>
#include <QFormLayout>
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
    QVector<PaletteColourWidget*> swatch;
    QFormLayout * outerLayout;
    FlowLayout  * paletteLayout;

    Palette     * palette;
    PaletteColourWidget * activeColourDisplay;
    PaletteColourWidget * selectedColour;

    void emptySwatch();

signals:
    void colourSelected(PaletteColour*);

public slots:
    void addColour();
    void selected(PaletteColourWidget*);
};

#endif // PALETTEWIDGET_H
