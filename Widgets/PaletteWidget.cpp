#include <QColorDialog>
#include <QPushButton>
#include <QVBoxLayout>

#include "FlowLayout.h"
#include "PaletteColour.h"
#include "PaletteWidget.h"

PaletteWidget::PaletteWidget(QWidget *parent)
    : QFrame(parent)
    , selectedColour(NULL)
{
    QPushButton* addColour = new QPushButton("+");

    QFrame* activeRegion = new QFrame();
    QFrame* paletteBox   = new QFrame();

    QVBoxLayout* activeRegionLayout = new QVBoxLayout();

    PaletteColour * colour = new PaletteColour(QColor(0, 0, 0));
    activeColourDisplay = new PaletteColourWidget(this, colour);
    connect(activeColourDisplay,SIGNAL(selected(PaletteColourWidget*)),
            this,		SLOT  (selected(PaletteColourWidget*)));

    addColour->setMaximumSize(activeColourDisplay->size, activeColourDisplay->size);
    connect(addColour,		SIGNAL(clicked()),
            this,		SLOT  (addColour()));

    activeRegionLayout->addWidget(addColour);
    activeRegionLayout->addWidget(activeColourDisplay);

    paletteLayout = new FlowLayout(1,1,1);
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
    if (!newColour.isValid()) {
        return;
    }
    // send this to the palette instead
    PaletteColour *      colour       = new PaletteColour(newColour);
    PaletteColourWidget* colourWidget = new PaletteColourWidget(this, colour);
    swatch.append(colourWidget);
    connect(colourWidget,	SIGNAL(selected(PaletteColourWidget*)),
            this,		SLOT  (selected(PaletteColourWidget*)));
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
        connect(colourWidget,	SIGNAL(selected(PaletteColourWidget*)),
                this,		SLOT  (selected(PaletteColourWidget*)));
        paletteLayout->addWidget(colourWidget);
        if (i == 0)  {
            selected(colourWidget);
        }
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

void PaletteWidget::selected(PaletteColourWidget* widget)
{
    if (widget != activeColourDisplay) {
        activeColourDisplay->setPaletteColour(widget->getPaletteColour());
        if (selectedColour == widget) {
            // if the active colour has been changed
            emit colourChanged(widget->getPaletteColour());
        }
        else {
            // otherwise we are just swapping things
            emit colourSelected(widget->getPaletteColour());
        }
        selectedColour = widget;
    }
    else {
        // changes the colour on the active widget
        selectedColour->setPaletteColour(widget->getPaletteColour());
        emit colourChanged(widget->getPaletteColour());
    }
}
