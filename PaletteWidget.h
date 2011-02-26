#ifndef PALETTEWIDGET_H
#define PALETTEWIDGET_H

#include <QFrame>
#include <QVBoxLayout>
#include <QVector>
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
    QVBoxLayout *outer_layout;

signals:
    void selectedColour(PaletteColour*);

public slots:
    void addColour();
    void selected(PaletteColour*);
};

#endif // PALETTEWIDGET_H
