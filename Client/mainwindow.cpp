#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), settings(new Settings)
{
    ui->setupUi(this);

    model        = new AddressBookModel(this);
    socketWorker = new SocketWorker(this);
    connect(model, &AddressBookModel::socketAddRow, socketWorker, &SocketWorker::socketAddRow);
    connect(model, &AddressBookModel::socketModifyRow, socketWorker, &SocketWorker::socketModifyRow);
    connect(model, &AddressBookModel::socketRemoveRow, socketWorker, &SocketWorker::socketRemoveRow);
    connect(socketWorker, &SocketWorker::modelClear, model, &AddressBookModel::modelClear);
    connect(socketWorker, &SocketWorker::modelAddRow, model, &AddressBookModel::modelAddRow);
    connect(socketWorker, &SocketWorker::modelModifyRow, model, &AddressBookModel::modelModifyRow);
    connect(socketWorker, &SocketWorker::modelRemoveRow, model, &AddressBookModel::modelRemoveRow);
    connect(socketWorker, &SocketWorker::socketStatusChanged, &socketStatus, &QLabel::setText);
    connect(socketWorker, &SocketWorker::socketSpeedChanged, &socketSpeed, &QLabel::setText);
    connect(settings, &Settings::addressOrPortChanged, socketWorker, &SocketWorker::addressOrPortChanged);
    ui->tableView->setModel(model);
    connect(ui->tableView, &QTableView::clicked, this, &MainWindow::tableViewClicked);

    ui->toolBar->addAction("AddRow", this, &MainWindow::addRow);
    ui->toolBar->addAction("DeleteRow", this, &MainWindow::deleteCurrentRow);
    ui->toolBar->addAction("Sync", this, &MainWindow::sync);
    QWidget *spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    ui->toolBar->addWidget(spacer);
    ui->toolBar->addAction("Settings", this, &MainWindow::showSettings);

    ui->statusBar->addWidget(&socketStatus);
    ui->statusBar->addWidget(&socketSpeed);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
        case Qt::Key_Delete:
            deleteCurrentRow();
            break;
        case Qt::Key_Insert:
            addRow();
            break;
    }
}

void MainWindow::addRow()
{
    model->insertRows(0, 1);
}

void MainWindow::deleteCurrentRow()
{
    ui->tableView->model()->removeRow(ui->tableView->selectionModel()->currentIndex().row());
}

void MainWindow::sync()
{
    socketWorker->socketSync();
}

void MainWindow::showSettings()
{
    settings->show();
}

void MainWindow::tableViewClicked(const QModelIndex &index)
{
    if (index.column() == 3)
    {
        if (model->data(index, Qt::DisplayRole).toString() == " ")
            model->setData(index, static_cast<int>(Person::Sex::Male));
        else if (model->data(index, Qt::DisplayRole).toString() == "♂")
            model->setData(index, static_cast<int>(Person::Sex::Female));
        else if (model->data(index, Qt::DisplayRole).toString() == "♀")
            model->setData(index, static_cast<int>(Person::Sex::Undefined));
        else
            model->setData(index, static_cast<int>(Person::Sex::Undefined));
    }
}
