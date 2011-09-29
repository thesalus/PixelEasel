#include <QColorDialog>
#include <QPushButton>
#include <QVBoxLayout>

#include "FlowLayout.h"
#include "PaletteColour.h"
#include "PaletteWidget.h"

PaletteWidget::PaletteWidget(QWidget *parent) :
    QFrame(parent)
{
    QPushButton* addColour = new QPushButton("+");

    QFrame* activeRegion = new QFrame();
    QFrame* paletteBox   = new QFrame();

    QVBoxLayout* activeRegionLayout = new QVBoxLayout();

    PaletteColour * colour = new PaletteColour(QColor(0, 0, 0));
    activeColour = new PaletteColourWidget(this, colour);
    connect(activeColour,	SIGNAL(selected(PaletteColour*)),
            this,		SLOT  (selected(PaletteColour*)));

    addColour->setMaximumSize(activeColour->size, activeColour->size);
    connect(addColour,		SIGNAL(clicked()),
            this,		SLOT  (addColour()));

    activeRegionLayout->addWidget(addColour);
    activeRegionLayout->addWidget(activeColour);

    paletteLayout = new FlowLayout();
    outerLayout   = new QFormLayout();

    activeRegion->setLayout(activeRegionLayout);
    paletteBox->setLayout(paletteLayout);
    outerLayout->insertRow(0, activeRegion, paletteBox);
    setLayout(outerLayout);
}

PaletteWidget::~PaletteWidget()
{
    emptySwatch();
}

void PaletteWidget::addColour()
{
    QColor newColour = QColorDialog::getColor();
    // send this to the palette instead
    PaletteColour *      colour       = new PaletteColour(newColour);
    PaletteColourWidget* colourWidget = new PaletteColourWidget(this, colour);
    swatch.append(colourWidget);
    connect(colourWidget,	SIGNAL(selected(PaletteColour*)),
            this,		SLOT  (selected(PaletteColour*)));
    paletteLayout->addWidget(colourWidget);
    emit colourSelected(colour);
}

void PaletteWidget::changeSwatch(Palette* newPalette)
{
    palette = newPalette;
    emptySwatch();

    QVector<PaletteColour*> *colours = palette->getColours();
    // what if the size is more than a reasonable amount?
    for (int i = 0; i < colours->size(); ++i) {
        PaletteColourWidget* colourWidget = new PaletteColourWidget(this, colours->at(i));
        swatch.append(colourWidget);
        connect(colourWidget,	SIGNAL(selected(PaletteColour*)),
                this,		SLOT  (selected(PaletteColour*)));
        paletteLayout->addWidget(colourWidget);
    }

    if (!colours->isEmpty()) {
        selected(colours->first());
    }
}

void PaletteWidget::emptySwatch()
{
    while (!swatch.isEmpty()) {
        PaletteColourWidget * widget = swatch.back();
        swatch.pop_back();
        paletteLayout->removeWidget(widget);
        delete widget;
    }
}

void PaletteWidget::selected(PaletteColour* colour)
{
    activeColour->setPaletteColour(colour);
    emit colourSelected(colour);
}
