/**
 * @file Controller.h
 * @brief Высокоуровневый контроллер для настройки клапана и связи с QML.
 *
 * Класс Controller владеет объектами низкоуровневого обмена данными
 * (@ref Data, @ref UART), управляет алгоритмом настройки через
 * @ref Insufflator и экспортирует своё состояние в QML через набор
 * свойств Q_PROPERTY. Этот класс — основная «точка входа» логики
 * приложения для QML-интерфейса.
 */

#pragma once

#include <QTimer>

#include "SendAndReadData.h"
#include "Insufflator.h"

/**
 * @brief Контроллер приложения, доступный из QML.
 *
 * Экземпляр этого класса создаётся в @ref main.cpp и пробрасывается
 * в QML под именем @c controller. Через свойства и методы Controller
 * QML-слой может:
 *  - выбирать COM-порт и управлять подключением,
 *  - запускать и останавливать алгоритм настройки,
 *  - читать текущие измерения (PWM, расход, ошибка),
 *  - получать калибровочные точки и результаты аппроксимации,
 *  - отображать человекочитаемый лог работы.
 */
class Controller : public QObject {
    Q_OBJECT

    /// Имя последовательного порта (например, "COM3"), выбранного пользователем.
    Q_PROPERTY(QString portName READ portName WRITE setPortName NOTIFY portNameChanged)
    /// Список доступных COM-портов, регулярно обновляемый в фоне.
    Q_PROPERTY(QStringList availablePorts READ availablePorts NOTIFY availablePortsChanged)
    /// @c true, если в данный момент установлено соединение с устройством.
    Q_PROPERTY(bool connected READ isConnected NOTIFY connectedChanged)
    /// @c true, если алгоритм настройки сейчас запущен.
    Q_PROPERTY(bool running READ isRunning NOTIFY runningChanged)

    /// Текущий PWM, рассчитанный алгоритмом.
    Q_PROPERTY(int pwm READ pwm NOTIFY valuesChanged)
    /// Текущее измеренное значение расхода.
    Q_PROPERTY(double flow READ flow NOTIFY valuesChanged)
    /// Текущая ошибка регулирования (расход - целевое значение).
    Q_PROPERTY(double error READ error NOTIFY valuesChanged)

    /// PWM первой калибровочной точки.
    Q_PROPERTY(int pwm1 READ pwm1 NOTIFY calibrationChanged)
    /// Расход первой калибровочной точки.
    Q_PROPERTY(double flow1 READ flow1 NOTIFY calibrationChanged)
    /// PWM второй калибровочной точки.
    Q_PROPERTY(int pwm2 READ pwm2 NOTIFY calibrationChanged)
    /// Расход второй калибровочной точки.
    Q_PROPERTY(double flow2 READ flow2 NOTIFY calibrationChanged)

    /// Наклон прямой аппроксимации (коэффициент перед расходом).
    Q_PROPERTY(double slope READ slope NOTIFY resultChanged)
    /// Смещение (offset) прямой аппроксимации.
    Q_PROPERTY(int offset READ offset NOTIFY resultChanged)

    /// Текстовый лог важных событий, отображаемый в интерфейсе.
    Q_PROPERTY(QString logText READ logText NOTIFY logTextChanged)

public:
    /// Создаёт новый контроллер с указанным родительским QObject.
    explicit Controller(QObject *parent = nullptr);

    /// Возвращает текущее имя выбранного порта.
    QString portName() const { return m_portName; }
    /// Устанавливает имя порта. Вызывает сигнал portNameChanged() при изменении.
    void setPortName(const QString &name);

    /// Возвращает список обнаруженных COM-портов.
    QStringList availablePorts() const { return m_availablePorts; }

    /// Возвращает @c true, если соединение с устройством открыто.
    bool isConnected() const { return m_connected; }
    /// Возвращает @c true, если алгоритм настройки запущен.
    bool isRunning() const { return m_running; }

    /// Возвращает текущий PWM.
    int pwm() const { return m_pwm; }
    /// Возвращает текущее значение расхода.
    double flow() const { return m_flow; }
    /// Возвращает текущую ошибку регулирования.
    double error() const { return m_error; }

    /// PWM для первой калибровочной точки.
    int pwm1() const { return m_inValue.PWM1; }
    /// Расход для первой калибровочной точки.
    double flow1() const { return m_inValue.FLOW1; }
    /// PWM для второй калибровочной точки.
    int pwm2() const { return m_inValue.PWM2; }
    /// Расход для второй калибровочной точки.
    double flow2() const { return m_inValue.FLOW2; }

    /// Возвращает текущий наклон прямой аппроксимации.
    double slope() const { return m_slope; }
    /// Возвращает текущее смещение прямой аппроксимации.
    int offset() const { return m_offset; }

    /// Возвращает накопленный текст лога.
    QString logText() const { return m_logText; }

    /// Подключается к устройству или отключается от него в зависимости от текущего состояния.
    Q_INVOKABLE void connectOrDisconnect();
    /// Запускает или останавливает алгоритм настройки в зависимости от текущего состояния.
    Q_INVOKABLE void startOrStop();
    /// Принудительно обновляет список доступных COM-портов.
    Q_INVOKABLE void refreshPorts();

signals:
    /// Сигнал об изменении имени порта.
    void portNameChanged();

    /// Сигнал об изменении списка доступных портов.
    void availablePortsChanged();

    /// Сигнал об изменении состояния соединения.
    void connectedChanged();

    /// Сигнал об изменении состояния алгоритма (запущен/остановлен).
    void runningChanged();

    /// Сигнал об изменении текущих значений PWM/расход/ошибка.
    void valuesChanged();

    /// Сигнал об изменении калибровочных точек.
    void calibrationChanged();

    /// Сигнал об изменении результата аппроксимации (наклон/смещение).
    void resultChanged();

    /// Сигнал об обновлении текстового лога.
    void logTextChanged();

    /// Сигнал о пользовательской ошибке, которую нужно показать в интерфейсе.
    void errorOccurred(const QString &message);

private slots:
    /// Периодически опрашивает алгоритм Insufflator и обновляет измерения.
    void updateInsufflatorData();

private:
    /// Добавляет строку в текстовый лог и испускает сигнал logTextChanged().
    void appendLog(const QString &line);

    /// Сбрасывает состояние измерений/калибровки в исходное.
    void resetMeasurement();

    QString m_portName;        ///< Выбранное имя последовательного порта.
    bool m_connected = false;  ///< Текущее состояние соединения с устройством.
    bool m_running = false;    ///< Флаг: алгоритм настройки запущен или нет.

    QTimer m_timer;       ///< Таймер, по которому вызывается обновление измерений.
    QTimer m_portsTimer;  ///< Таймер периодического сканирования COM-портов.

    Data *m_data = nullptr;              ///< Обёртка над UART с протоколом устройства.
    Insufflator *m_in = nullptr;         ///< Текущий экземпляр алгоритма Insufflator.
    Insufflator::INValue m_inValue{};    ///< Сохранённые калибровочные точки.

    double m_flowTarget = 2.0;  ///< Целевое значение расхода в текущей фазе.

    int m_pwm = 0;          ///< Последнее вычисленное значение PWM.
    double m_flow = 0.0;    ///< Последнее измеренное значение расхода.
    double m_error = 0.0;   ///< Последняя ошибка регулирования.

    double m_slope = 0.0;   ///< Наклон аппроксимирующей зависимости.
    int m_offset = 0;       ///< Смещение аппроксимирующей зависимости.

    QString m_logText;      ///< Сборный текстовый лог для отображения в UI.

    QStringList m_availablePorts; ///< Кэшированный список доступных последовательных портов.
};
