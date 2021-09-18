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

    ui->toolBar->addAction("Pushback Person", this, &MainWindow::pushBackRow);
    ui->toolBar->addAction("Pushfront Person", this, &MainWindow::pushFrontRow);
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
            pushBackRow();
            break;
    }
}

void MainWindow::pushBackRow()
{
    model->insertRows(model->rowCount(), 1);
}

void MainWindow::pushFrontRow()
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
