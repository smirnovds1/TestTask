#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QRegularExpressionValidator>
#include <QSettings>
#include <QTimer>

namespace Ui
{
class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = nullptr);
    ~Settings();

signals:
    void addressOrPortChanged(const QString &address, const QString &port);

private slots:
    void on_buttonBox_accepted();

private:
    Ui::Settings *ui;
    QSettings *settings;
    QString address;
    QString port;
};

#endif // SETTINGS_H
