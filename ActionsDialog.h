#ifndef ACTIONSDIALOG_H
#define ACTIONSDIALOG_H

#include <QDialog>
#include <QAction>
#include <QList>
#include <QTableWidget>

class ActionsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ActionsDialog(QList<QAction*> actions, QWidget *parent = 0);

signals:

protected slots:
    void accept();


private slots:
    void recordAction(int row, int column);
    void validateAction(int row, int column);

private:
    QString          old_shortcut;
    QTableWidget    *actionsTable;
    QList<QAction*>  actionsList;

};

#endif // ACTIONSDIALOG_H
