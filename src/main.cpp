#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "Controller.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);

    Controller controller;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(
        QStringLiteral("controller"), &controller);

    engine.loadFromModule("ValveTuner", "Main");

    return app.exec();
}
