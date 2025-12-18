/**
 * @file main.cpp
 * @brief Точка входа приложения Valve Tuner.
 *
 * В этом файле создаётся объект Qt-приложения, основной класс
 * логики @ref Controller и QML-движок. Объект контроллера
 * пробрасывается в QML под именем контекстного свойства @c controller,
 * после чего загружается главный QML-файл модуля @c ValveTuner.
 */

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "Controller.h"

/**
 * @brief Точка входа в программу.
 * @param argc Количество аргументов командной строки.
 * @param argv Массив аргументов командной строки.
 * @return Код завершения цикла обработки событий Qt.
 */
int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);

    Controller controller;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(
        QStringLiteral("controller"), &controller);

    engine.loadFromModule("ValveTuner", "Main");

    return app.exec();
}
