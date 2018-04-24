#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "qml_port_list.h"
#include "qml_relay.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    qmlRegisterType <CPortList>("com.dogratian.qml.PortList", 1, 0, "PortList");
    qmlRegisterType <CRelay>("com.dogratian.qml.Relay", 1, 0, "Relay");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
