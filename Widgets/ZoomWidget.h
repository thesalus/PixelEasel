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
    QSlider        *zoomSlider;
    QHBoxLayout    *outerLayout;
    QComboBox      *zoomDropdown;

    int             n_levels;

    int             currentLevel;    // defaults to 1
    QString        *descriptions;
};

#endif // ZOOMWIDGET_H
