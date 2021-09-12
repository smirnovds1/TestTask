#include "settings.h"
#include "ui_settings.h"

Settings::Settings(QWidget *parent) : QDialog(parent), ui(new Ui::Settings)
{
    ui->setupUi(this);
    settings = new QSettings("client.ini", QSettings::IniFormat);
    ui->addressLineEdit->setValidator(new QRegularExpressionValidator(QRegularExpression(
    "(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])")));
    ui->portLineEdit->setValidator(new QRegularExpressionValidator(QRegularExpression(
    "^6553[0-5]|655[0-2][0-9]|65[0-4][0-9]{2}|6[0-4][0-9]{3}|[0-5][0-9]{4}|[1-9][0-9]{1,3}|[1-9]$")));
    address = settings->value("address", "127.0.0.1").toString();
    port    = settings->value("port", "31337").toString();
    ui->addressLineEdit->setText(address);
    ui->portLineEdit->setText(port);
    QTimer::singleShot(0, this, [this]() { emit addressOrPortChanged(address, port); });
}

Settings::~Settings()
{
    delete ui;
    delete settings;
}

void Settings::on_buttonBox_accepted()
{
    settings->setValue("address", ui->addressLineEdit->text());
    settings->setValue("port", ui->portLineEdit->text());
    bool addressChanged = false;
    bool portChanged    = false;
    if (address != ui->addressLineEdit->text())
    {
        addressChanged = true;
        address        = ui->addressLineEdit->text();
    }
    if (port != ui->portLineEdit->text())
    {
        portChanged = true;
        port        = ui->portLineEdit->text();
    }
    if (addressChanged || portChanged)
        emit addressOrPortChanged(address, port);
}
