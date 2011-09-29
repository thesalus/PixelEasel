#include "ActionsDialog.h"
#include <QGridLayout>
#include <QHeaderView>
#include <QListIterator>
#include <QPushButton>

ActionsDialog::ActionsDialog(QList<QAction*> actions,
                             QWidget *parent)
    : QDialog(parent)
{
    this->actionsTable = new QTableWidget(actions.count(), 2, this);
    QStringList headerLabels;
    headerLabels << tr("Description");
    headerLabels << tr("Shortcut");
    this->actionsTable->setHorizontalHeaderLabels(headerLabels);
    this->actionsTable->verticalHeader()->setVisible(false);


    QAction *action;
    int rowIndex = 0;
    QTableWidgetItem *item;
    for (QListIterator<QAction*> i(actions); i.hasNext(); action = i.next()) {
        item = new QTableWidgetItem(action->text());
        item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        this->actionsTable->setItem(rowIndex, 0, item);
        item = new QTableWidgetItem(action->shortcut().toString());
        this->actionsTable->setItem(rowIndex, 1, item);
        this->actionsList.append(action);
        rowIndex++;
    }

    QPushButton *saveButton   = new QPushButton(tr("&OK"), this);
    QPushButton *cancelButton = new QPushButton(tr("&Cancel"), this);
    connect(saveButton,		SIGNAL(clicked()),
            this,		SLOT  (accept()));
    connect(cancelButton,	SIGNAL(clicked()),
            this, 		SLOT  (reject()));

    connect(this->actionsTable,	SIGNAL(currentCellChanged(int, int)),
            this, 		SLOT  (recordAction(int, int)));
    connect(this->actionsTable,	SIGNAL(cellChanged(int, int)),
            this, 		SLOT  (validateAction(int, int)));

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    mainLayout->addWidget(this->actionsTable, 0, 0, 2, 5);
    mainLayout->addWidget(saveButton,         5, 0);
    mainLayout->addWidget(cancelButton,       5, 1);
    setLayout(mainLayout);
    setWindowTitle(tr("Edit Actions"));
}

void ActionsDialog::recordAction(int row, int col)
{
    this->oldShortcut = this->actionsTable->item(row, col)->text();
}

void ActionsDialog::validateAction(int row, int column)
{
    QTableWidgetItem *item = this->actionsTable->item(row, column);
    QString shortcut = QString(QKeySequence(item->text()));

    bool isInvalid = shortcut.isEmpty() && !item->text().isEmpty();
    if (isInvalid) {
        item->setText(this->oldShortcut);
    }
    else {
        item->setText(shortcut);
    }
}

void ActionsDialog::accept()
{
    for (int row = 0; row < this->actionsList.size(); ++row) {
        QAction *action = this->actionsList[row];
        action->setShortcut(QKeySequence(this->actionsTable->item(row, 1)->text()));
    }

    QDialog::accept();
}
