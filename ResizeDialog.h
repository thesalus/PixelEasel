#ifndef RESIZEDIALOG_H
#define RESIZEDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QSpinBox>
#include <QPushButton>
#include "ImageDocument.h"

// For extensions: http://doc.qt.nokia.com/4.6/dialogs-extension.html

class ResizeDialog : public QDialog
{
    Q_OBJECT

public:
    ResizeDialog(QSize size, QWidget* parent = 0);
    QSize getSize();

private slots:
    void acceptSize();
    void rejectSize();

protected:
    QSize           original_size;
    QSize           return_size;
    QLabel         *width_label;
    QLabel         *height_label;
    QSpinBox       *width_input;
    QSpinBox       *height_input;
    QPushButton    *resize_button;
    QPushButton    *cancel_button;
};

#endif // RESIZEDIALOG_H
