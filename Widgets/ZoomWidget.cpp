#include "ZoomWidget.h"

/* The Zoom widget is a thin bar that allows the user to
 * adjust the zoom controls of the current canvas. It is
 * comprised of two parts, one a dropdown that allows for
 * selection of a chosen few zoom levels, while the slider
 * serves much the same purpose, but makes it easier to
 * switch between various scales. The zoom level of each
 * document should be independent of each other.
 */

ZoomWidget::ZoomWidget(int levels, QString descriptions[]) :
        n_levels(levels)
        , m_current_level(1)
        , m_descriptions(descriptions)
{
    Q_ASSERT(levels > 0);

    i_zoom_slider = new QSlider(Qt::Horizontal, this);
    i_zoom_slider->setRange(0, levels-1);
    i_zoom_slider->setTracking(true);
    i_zoom_slider->setTickPosition (QSlider::TicksAbove);
    connect(i_zoom_slider,  SIGNAL(valueChanged(int)),
            this,           SLOT(setValue(int)));
    i_zoom_dropdown = new QComboBox(this);
    for (int i = 0; i < levels; i++)
    {
        i_zoom_dropdown->insertItem(i, descriptions[i]);
    }
    connect(i_zoom_dropdown,    SIGNAL(currentIndexChanged(int)),
            this,               SLOT(setValue(int)));


    i_outer_layout = new QHBoxLayout;
        i_outer_layout->addWidget(i_zoom_dropdown);
        i_outer_layout->addWidget(i_zoom_slider);
    setLayout(i_outer_layout);
}

void ZoomWidget::setValue(int level)
{
    if (level != m_current_level)
    {
        if (level > 7)
            level = 7;
        else if (level < 0)
            level = 0;
        m_current_level = level;
        i_zoom_slider->setValue(level);
        i_zoom_dropdown->setCurrentIndex(level);
        emit valueChanged(level);
    }
}

QSize ZoomWidget::sizeHint() const
{
    return QSize(200, 40);
}
