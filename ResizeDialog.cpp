#include "ResizeDialog.h"
#include <QGridLayout>
#include <iostream>
using namespace std;

ResizeDialog::ResizeDialog(QSize size, QWidget* parent)
    : QDialog(parent), original_size(size), return_size(size)
{
    width_label = new QLabel(tr("&Width:"));
    width_input = new QSpinBox();
    width_input->setRange(1,INT_MAX);
    width_input->setValue(size.width());
    width_label->setBuddy(width_input);

    height_label = new QLabel(tr("&Height:"));
    height_input = new QSpinBox();
    height_input->setRange(1,INT_MAX);
    height_input->setValue(size.height());
    height_label->setBuddy(height_input);

    resize_button = new QPushButton(tr("&Okay"));
    resize_button->setDefault(true);
    connect(resize_button, SIGNAL(clicked()), this, SLOT(acceptSize()));

    cancel_button = new QPushButton(tr("&Cancel"));
    connect(cancel_button, SIGNAL(clicked()), this, SLOT(rejectSize()));

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    mainLayout->addWidget(width_label, 0, 0);
    mainLayout->addWidget(width_input, 0, 1);
    mainLayout->addWidget(height_label, 1, 0);
    mainLayout->addWidget(height_input, 1, 1);
    mainLayout->addWidget(resize_button, 2, 0);
    mainLayout->addWidget(cancel_button, 2, 1);
    setLayout(mainLayout);

    setWindowTitle(tr("Resize Image"));
}

void ResizeDialog::acceptSize()
{
    return_size = QSize(width_input->value(), height_input->value());
    this->hide();
}

void ResizeDialog::rejectSize()
{
    this->hide();
}

QSize ResizeDialog::getSize()
{
    return return_size;
}
