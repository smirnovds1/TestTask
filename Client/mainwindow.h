#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QKeyEvent>
#include <QLabel>
#include <QMainWindow>

#include "addressbookmodel.h"
#include "settings.h"
#include "socketworker.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void addRow();
    void deleteCurrentRow();
    void sync();
    void showSettings();

private:
    Ui::MainWindow *ui;
    Settings *settings;

    QLabel socketStatus;
    QLabel socketSpeed;

    AddressBookModel *model;
    SocketWorker *socketWorker;
};
#endif // MAINWINDOW_H
