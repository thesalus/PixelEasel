#include "ResizeDialog.h"
#include <QGridLayout>
#include <iostream>
using namespace std;

ResizeDialog::ResizeDialog(QSize size, QWidget* parent)
    : QDialog(parent), originalSize(size), returnSize(size)
{
    widthLabel = new QLabel(tr("&Width:"));
    widthInput = new QSpinBox();
    widthInput->setRange(1,INT_MAX);
    widthInput->setValue(size.width());
    widthLabel->setBuddy(widthInput);

    heightLabel = new QLabel(tr("&Height:"));
    heightInput = new QSpinBox();
    heightInput->setRange(1,INT_MAX);
    heightInput->setValue(size.height());
    heightLabel->setBuddy(heightInput);

    resizeButton = new QPushButton(tr("&Okay"));
    resizeButton->setDefault(true);
    connect(resizeButton, SIGNAL(clicked()), this, SLOT(acceptSize()));

    cancelButton = new QPushButton(tr("&Cancel"));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(rejectSize()));

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    mainLayout->addWidget(widthLabel, 0, 0);
    mainLayout->addWidget(widthInput, 0, 1);
    mainLayout->addWidget(heightLabel, 1, 0);
    mainLayout->addWidget(heightInput, 1, 1);
    mainLayout->addWidget(resizeButton, 2, 0);
    mainLayout->addWidget(cancelButton, 2, 1);
    setLayout(mainLayout);

    setWindowTitle(tr("Resize Image"));
}

void ResizeDialog::acceptSize()
{
    returnSize = QSize(widthInput->value(), heightInput->value());
    this->hide();
}

void ResizeDialog::rejectSize()
{
    this->hide();
}

QSize ResizeDialog::getSize()
{
    return returnSize;
}
