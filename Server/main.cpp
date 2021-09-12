#include <QCommandLineParser>
#include <QCoreApplication>
#include <signal.h>

#include "server.h"

void sigHandler(int)
{
    qApp->quit();
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    signal(SIGINT, &sigHandler);
    signal(SIGTERM, &sigHandler);

    QCommandLineParser parser;
    parser.addHelpOption();
    QCommandLineOption portOption(QStringList() << "p"
                                                << "port",
                                  "server port",
                                  "port");
    parser.addOption(portOption);
    parser.process(a);

    quint16 port = 31337;
    if (parser.isSet("port"))
        port = parser.value("port").toUInt();

    Server server(port);

    return a.exec();
}
