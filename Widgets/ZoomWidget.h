#ifndef ZOOMWIDGET_H
#define ZOOMWIDGET_H

#include <QComboBox>
#include <QFrame>
#include <QHBoxLayout>
#include <QSlider>

class ZoomWidget : public QFrame
{
    Q_OBJECT
public:
    ZoomWidget(int, QString[]);
    QSize sizeHint() const;

public slots:
    void setValue(int level);

signals:
    void valueChanged(int);

private:
    // Interface Elements
    QSlider        *i_zoom_slider;
    QHBoxLayout    *i_outer_layout;
    QComboBox      *i_zoom_dropdown;

    // Counts
    int             n_levels;

    // Class Members
    int             m_current_level;    // defaults to 1
    QString        *m_descriptions;
};

#endif // ZOOMWIDGET_H
