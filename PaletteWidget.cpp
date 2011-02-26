#include "PaletteWidget.h"
#include "PaletteColour.h"
#include <QPushButton>
#include <QColorDialog>

PaletteWidget::PaletteWidget(QWidget *parent) :
    QFrame(parent)
{
    QPushButton* add_colour = new QPushButton("+");
    outer_layout = new QVBoxLayout();
    outer_layout->addWidget(add_colour);
    setLayout(outer_layout);
    connect(add_colour, SIGNAL(clicked()), this, SLOT(addColour()));
    // initialize it with 1 colour... black
}

PaletteWidget::~PaletteWidget()
{
}

void PaletteWidget::addColour()
{
    QColor new_colour = QColorDialog::getColor();
    // send this to the palette
    PaletteColour *colour = new PaletteColour(new_colour);
    PaletteColourWidget* colour_widget = new PaletteColourWidget(this, colour);
    swatch.append(colour_widget);
    connect(colour_widget,	SIGNAL(selected(PaletteColour*)),
            this,           SLOT(selected(PaletteColour*)));
    outer_layout->addWidget(colour_widget);
}

void PaletteWidget::changeSwatch(Palette* palette_)
{
    palette = palette_;
    QVector<PaletteColour*> *colours = palette->getColours();
    // do we need to explicitly delete them?
    swatch.clear();

    // what if the size is more than a reasonable amount?
    for (int i = 0; i < colours->size(); ++i) {
        PaletteColourWidget* colour_widget = new PaletteColourWidget(this, colours->at(i));
        swatch.append(colour_widget);
        connect(colour_widget,	SIGNAL(selected(PaletteColour*)),
                this,           SLOT(selected(PaletteColour*)));
        outer_layout->addWidget(colour_widget);
    }
}

void PaletteWidget::selected(PaletteColour* colour)
{
    emit selectedColour(colour);
}
