#include "PaletteWidget.h"
#include "PaletteColour.h"
#include <QPushButton>
#include <QColorDialog>

PaletteWidget::PaletteWidget(QWidget *parent) :
    QFrame(parent)
{
    QPushButton* add_colour = new QPushButton("+");
    outer_layout = new FlowLayout;
    outer_layout->addWidget(add_colour);
    setLayout(outer_layout);
    connect(add_colour,	SIGNAL(clicked()),
            this,	SLOT(addColour()));
}

PaletteWidget::~PaletteWidget()
{
    for (int i = 0; i < swatch.size(); ++i) {
        delete swatch.at(i);
    }
}

void PaletteWidget::addColour()
{
    QColor new_colour = QColorDialog::getColor();
    // send this to the palette instead
    PaletteColour *colour = new PaletteColour(new_colour);
    PaletteColourWidget* colour_widget = new PaletteColourWidget(this, colour);
    swatch.append(colour_widget);
    connect(colour_widget,	SIGNAL(selected(PaletteColour*)),
            this,		SLOT(selected(PaletteColour*)));
    outer_layout->addWidget(colour_widget);
    emit selectedColour(colour);
}

void PaletteWidget::changeSwatch(Palette* palette_)
{
    palette = palette_;
    QVector<PaletteColour*> *colours = palette->getColours();
    // do we need to explicitly delete them?

    while (!swatch.isEmpty()) {
        PaletteColourWidget*widget = swatch.back();
        swatch.pop_back();
        outer_layout->removeWidget(widget);
        delete widget;
    }

    // what if the size is more than a reasonable amount?
    for (int i = 0; i < colours->size(); ++i) {
        PaletteColourWidget* colour_widget = new PaletteColourWidget(this, colours->at(i));
        swatch.append(colour_widget);
        connect(colour_widget,	SIGNAL(selected(PaletteColour*)),
                this,		SLOT(selected(PaletteColour*)));
        outer_layout->addWidget(colour_widget);
    }
}

void PaletteWidget::selected(PaletteColour* colour)
{
    emit selectedColour(colour);
}
