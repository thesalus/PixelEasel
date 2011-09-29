#include "ZoomWidget.h"

/* The Zoom widget is a thin bar that allows the user to
 * adjust the zoom controls of the current canvas. It is
 * comprised of two parts, one a dropdown that allows for
 * selection of a chosen few zoom levels, while the slider
 * serves much the same purpose, but makes it easier to
 * switch between various scales. The zoom level of each
 * document should be independent of each other.
 */

ZoomWidget::ZoomWidget(int levels, QString descriptions[])
    : n_levels(levels)
    , currentLevel(1)
    , descriptions(descriptions)
{
    Q_ASSERT(levels > 0);

    zoomSlider = new QSlider(Qt::Horizontal, this);
    zoomSlider->setRange(0, levels-1);
    zoomSlider->setTracking(true);
    zoomSlider->setTickPosition (QSlider::TicksAbove);
    connect(zoomSlider,     SIGNAL(valueChanged(int)),
            this,           SLOT  (setValue(int)));
    zoomDropdown = new QComboBox(this);
    for (int i = 0; i < levels; i++) {
        zoomDropdown->insertItem(i, descriptions[i]);
    }
    connect(zoomDropdown,    SIGNAL(currentIndexChanged(int)),
            this,            SLOT  (setValue(int)));

    outerLayout = new QHBoxLayout;
        outerLayout->addWidget(zoomDropdown);
        outerLayout->addWidget(zoomSlider);
    setLayout(outerLayout);
}

void ZoomWidget::setValue(int level)
{
    if (level != currentLevel) {
        if (level > 7) {
            level = 7;
        }
        else if (level < 0) {
            level = 0;
        }
        currentLevel = level;
        zoomSlider->setValue(level);
        zoomDropdown->setCurrentIndex(level);
        emit valueChanged(level);
    }
}

QSize ZoomWidget::sizeHint() const
{
    return QSize(200, 40);
}
