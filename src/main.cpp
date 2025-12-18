#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "Controller.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    Controller controller;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(
        QStringLiteral("controller"), &controller);

    // Убираем жёсткий выход по objectCreationFailed, чтобы не получать -1
    engine.loadFromModule("ValveTuner", "Main");

    return app.exec();
}
