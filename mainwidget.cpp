#include <QTableView>
#include <QHeaderView>
#include <QtWidgets>

#include "mainwidget.h"

MainWidget::MainWidget()
{
    currentClientsTable = new CurrentClientsTable(this);
    queueTable = new QueueTable(this);
    setupCurrentClientsTable();
    setupQueueTable();
}

MainWidget::~MainWidget()
{

}

Person MainWidget::getPerson(int row)
{
    Person person;
    if(currentIndex() == 0) {
        QModelIndex nameIndex = currentClientsTable->index(row, 0, QModelIndex());
        QVariant varName = currentClientsTable->data(nameIndex, Qt::DisplayRole);
        person.setName(varName.toString());
        QModelIndex startDateIndex = currentClientsTable->index(row, 1, QModelIndex());
        QVariant varStartDate = currentClientsTable->data(startDateIndex, Qt::DisplayRole);
        person.setStartingDate(varStartDate.toDate());
        QModelIndex endDateIndex = currentClientsTable->index(row, 2, QModelIndex());
        QVariant varEndDate = currentClientsTable->data(endDateIndex, Qt::DisplayRole);
        person.setEndingDate(varEndDate.toDate());
    }
    else if(currentIndex() == 1) {
        QModelIndex nameIndex = queueTable->index(row, 1, QModelIndex());
        QVariant varName = queueTable->data(nameIndex, Qt::DisplayRole);
        person.setName(varName.toString());
    }

    return person;
}

void MainWidget::setupCurrentClientsTable()
{
    currentClientsProxyModel = new QSortFilterProxyModel(this);
    currentClientsProxyModel->setSourceModel(currentClientsTable);

    QTableView *tableView = new QTableView;
    tableView->setModel(currentClientsProxyModel);

    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->horizontalHeader()->setStretchLastSection(true);
    tableView->verticalHeader()->hide();
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);

    tableView->setSortingEnabled(true);

    connect(tableView->selectionModel(),
        &QItemSelectionModel::selectionChanged,
        this, &MainWidget::selectionChanged);

    connect(this, &QTabWidget::currentChanged, this, [this](int tabIndex) {
        auto *tableView = qobject_cast<QTableView *>(widget(tabIndex));
        if (tableView)
            emit selectionChanged(tableView->selectionModel()->selection());
    });

    addTab(tableView, "Henkilöt");
}

void MainWidget::setupQueueTable()
{
    queuedClientsProxyModel = new QSortFilterProxyModel(this);
    queuedClientsProxyModel->setSourceModel(queueTable);

    QTableView *tableView = new QTableView;
    tableView->setModel(queuedClientsProxyModel);

    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->horizontalHeader()->setStretchLastSection(true);
    tableView->verticalHeader()->hide();
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);

    tableView->setSortingEnabled(true);

    connect(tableView->selectionModel(),
        &QItemSelectionModel::selectionChanged,
        this, &MainWidget::selectionChanged);

    connect(this, &QTabWidget::currentChanged, this, [this](int tabIndex) {
        auto *tableView = qobject_cast<QTableView *>(widget(tabIndex));
        if (tableView)
            emit selectionChanged(tableView->selectionModel()->selection());
    });

    addTab(tableView, "Jono");
}

void MainWidget::showAddDialog()
{
    AddDialog aDialog;
    if(aDialog.exec()){
        Person person;
        person.setName(aDialog.nameEdit->text());
        person.setStartingDate(aDialog.startingDate->date());
        person.setEndingDate(aDialog.endingDate->date());

        if(currentIndex() == 0)
            addPerson(person);
        else if(currentIndex() == 1)
            addPersonToQueue(person);
    }
}

void MainWidget::addPerson(const Person person)
{
    currentClientsTable->insertRows(0, 1, QModelIndex());

    QModelIndex index = currentClientsTable->index(0, 0, QModelIndex());
    currentClientsTable->setData(index, person.getName(), Qt::EditRole);
    index = currentClientsTable->index(0, 1, QModelIndex());
    currentClientsTable->setData(index, person.getStartingDate(), Qt::EditRole);
    index = currentClientsTable->index(0, 2, QModelIndex());
    currentClientsTable->setData(index, person.getEndingDate(), Qt::EditRole);
}

void MainWidget::addPersonToQueue(const Person person)
{
    queueTable->insertRows(queueTable->getListOfPeople().size(), 1, QModelIndex());

    QModelIndex index = queueTable->index(queueTable->getListOfPeople().size() - 1, 1, QModelIndex());
    queueTable->setData(index, person.getName(), Qt::EditRole);
}

void MainWidget::editPerson()
{
    QTableView *temp = static_cast<QTableView*>(currentWidget());
    QSortFilterProxyModel *proxy = static_cast<QSortFilterProxyModel*>(temp->model());
    QItemSelectionModel *selectionModel = temp->selectionModel();
    QModelIndexList indexes = selectionModel->selectedRows();

    foreach (QModelIndex index, indexes) {
        int row = proxy->mapToSource(index).row();

        Person person = getPerson(row);
        editValues(person, row);
    }
}

void MainWidget::removePerson()
{
    QTableView *temp = static_cast<QTableView*>(currentWidget());
    QSortFilterProxyModel *proxy = static_cast<QSortFilterProxyModel*>(temp->model());
    QItemSelectionModel *selectionModel = temp->selectionModel();

    QModelIndexList indexes = selectionModel->selectedRows();

    foreach (QModelIndex index, indexes) {
        int row = proxy->mapToSource(index).row();
        if(currentIndex() == 0)
            currentClientsTable->removeRows(row, 1, QModelIndex());
        else if(currentIndex() == 1)
            queueTable->removeRows(row, 1, QModelIndex());
    }
}

void MainWidget::editValues(Person oldValues, int row)
{
    AddDialog editDialog;
    editDialog.setWindowTitle(tr("Muokkaa Henkilöä"));

    editDialog.nameEdit->setText(oldValues.getName());
    editDialog.startingDate->setDate(oldValues.getStartingDate());
    editDialog.endingDate->setDate(oldValues.getEndingDate());

    if(editDialog.exec()) {
        Person newValues;
        if(currentIndex() == 0) {
            newValues.setName(editDialog.nameEdit->text());
            if(newValues.getName() != oldValues.getName()) {
                QModelIndex index = currentClientsTable->index(row, 0, QModelIndex());
                currentClientsTable->setData(index, QVariant(newValues.getName()), Qt::EditRole);
            }
            newValues.setStartingDate(editDialog.startingDate->date());
            if(newValues.getStartingDate() != oldValues.getStartingDate()) {
                QModelIndex index = currentClientsTable->index(row, 1, QModelIndex());
                currentClientsTable->setData(index, QVariant(newValues.getStartingDate()), Qt::EditRole);
            }
            newValues.setEndingDate(editDialog.endingDate->date());
            if(newValues.getEndingDate() != oldValues.getEndingDate()) {
                QModelIndex index = currentClientsTable->index(row, 2, QModelIndex());
                currentClientsTable->setData(index, QVariant(newValues.getEndingDate()), Qt::EditRole);
            }
        }
        if(currentIndex() == 1) {
            newValues.setName(editDialog.nameEdit->text());
            if(newValues.getName() != oldValues.getName()) {
                QModelIndex index = queueTable->index(row, 1, QModelIndex());
                queueTable->setData(index, QVariant(newValues.getName()), Qt::EditRole);
            }
        }
    }
}
