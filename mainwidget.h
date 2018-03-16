#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "currentclientstable.h"
#include "queuedclientstable.h"

#include <QTabWidget>
#include <QSortFilterProxyModel>
#include <QItemSelection>

class MainWindow;
class QSortFilterProxyModel;
class QAbstractTableModel;

class MainWidget : public QTabWidget
{
    Q_OBJECT

public:
    MainWidget(MainWindow *parent = 0);
    ~MainWidget();

signals:
    void selectionChanged(const QItemSelection &selected);

private:
    void setupCurrentClientsTable();
    void setupQueuedClientsTable();

    QWidget *parent;
    CurrentClientsTable *currentClientsTable;
    QueuedClientsTable *queuedClientsTable;
    QSortFilterProxyModel *currentClientsProxyModel;
    QSortFilterProxyModel *queuedClientsProxyModel;
};

#endif // MAINWIDGET_H
