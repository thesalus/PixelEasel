#include "ActionsDialog.h"
#include <QGridLayout>
#include <QHeaderView>
#include <QListIterator>
#include <QPushButton>

ActionsDialog::ActionsDialog(QList<QAction*> actions,
                             QWidget *parent)
    : QDialog(parent)
{
    actionsTable = new QTableWidget(actions.count(), 2, this);
    QStringList header_labels;
    header_labels << tr("Description");
    header_labels << tr("Shortcut");
    actionsTable->setHorizontalHeaderLabels(header_labels);
    actionsTable->verticalHeader()->setVisible(false);

    QListIterator<QAction*> i(actions);
    QAction *action;
    int row = 0;
    QTableWidgetItem *item;
    while (i.hasNext())
    {
        action = i.next();
        item = new QTableWidgetItem(action->text());
        item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        actionsTable->setItem(row, 0, item);
        item = new QTableWidgetItem(action->shortcut().toString());
        actionsTable->setItem(row, 1, item);
        actionsList.append(action);
        ++row;
    }

    QPushButton *save_button = new QPushButton(tr("&OK"), this);
    QPushButton *cancel_button = new QPushButton(tr("&Cancel"), this);
    connect(save_button, SIGNAL(clicked()),
            this, SLOT(accept()));
    connect(cancel_button, SIGNAL(clicked()),
            this, SLOT(reject()));

    connect(actionsTable, SIGNAL(currentCellChanged(int, int)),
            this, SLOT(recordAction(int, int)));
    connect(actionsTable, SIGNAL(cellChanged(int, int)),
            this, SLOT(validateAction(int, int)));

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    mainLayout->addWidget(actionsTable, 0, 0, 2, 5);
    mainLayout->addWidget(save_button, 5, 0);
    mainLayout->addWidget(cancel_button, 5, 1);
    setLayout(mainLayout);
    setWindowTitle(tr("Edit Actions"));
}

void ActionsDialog::recordAction(int row, int col)
{
    old_shortcut = actionsTable->item(row, col)->text();
}

void ActionsDialog::validateAction(int row, int column)
{
    QTableWidgetItem *item = actionsTable->item(row, column);
    QString shortcut = QString(QKeySequence(item->text()));

    if (shortcut.isEmpty() && !item->text().isEmpty()) {
        item->setText(old_shortcut);
    } else {
        item->setText(shortcut);
    }
}

void ActionsDialog::accept()
{
    for (int row = 0; row < actionsList.size(); ++row) {
        QAction *action = actionsList[row];
        action->setShortcut(QKeySequence(actionsTable->item(row, 1)->text()));
    }

    QDialog::accept();
}
