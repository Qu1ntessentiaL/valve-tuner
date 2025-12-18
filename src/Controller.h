#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QTimer>

#include "SendandReadData.h"
#include "Insufflator.h"

class Controller : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString portName READ portName WRITE setPortName NOTIFY portNameChanged)
    Q_PROPERTY(bool connected READ isConnected NOTIFY connectedChanged)
    Q_PROPERTY(bool running READ isRunning NOTIFY runningChanged)

    Q_PROPERTY(int pwm READ pwm NOTIFY valuesChanged)
    Q_PROPERTY(double flow READ flow NOTIFY valuesChanged)
    Q_PROPERTY(double error READ error NOTIFY valuesChanged)

    Q_PROPERTY(int pwm1 READ pwm1 NOTIFY calibrationChanged)
    Q_PROPERTY(double flow1 READ flow1 NOTIFY calibrationChanged)
    Q_PROPERTY(int pwm2 READ pwm2 NOTIFY calibrationChanged)
    Q_PROPERTY(double flow2 READ flow2 NOTIFY calibrationChanged)

    Q_PROPERTY(double slope READ slope NOTIFY resultChanged)
    Q_PROPERTY(int offset READ offset NOTIFY resultChanged)

    Q_PROPERTY(QString logText READ logText NOTIFY logTextChanged)

public:
    explicit Controller(QObject *parent = nullptr);

    QString portName() const { return m_portName; }
    void setPortName(const QString &name);

    bool isConnected() const { return m_connected; }
    bool isRunning() const { return m_running; }

    int pwm() const { return m_pwm; }
    double flow() const { return m_flow; }
    double error() const { return m_error; }

    int pwm1() const { return m_inValue.PWM1; }
    double flow1() const { return m_inValue.FLOW1; }
    int pwm2() const { return m_inValue.PWM2; }
    double flow2() const { return m_inValue.FLOW2; }

    double slope() const { return m_slope; }
    int offset() const { return m_offset; }

    QString logText() const { return m_logText; }

    Q_INVOKABLE void connectOrDisconnect();
    Q_INVOKABLE void startOrStop();

signals:
    void portNameChanged();
    void connectedChanged();
    void runningChanged();

    void valuesChanged();
    void calibrationChanged();
    void resultChanged();

    void logTextChanged();
    void errorOccurred(const QString &message);

private slots:
    void updateInsufflatorData();

private:
    void appendLog(const QString &line);
    void resetMeasurement();

    QString m_portName;
    bool m_connected = false;
    bool m_running = false;

    QTimer m_timer;

    Data *m_data = nullptr;
    insufflator *m_in = nullptr;
    insufflator::INValue m_inValue{};

    double m_flowTarget = 2.0;

    int m_pwm = 0;
    double m_flow = 0.0;
    double m_error = 0.0;

    double m_slope = 0.0;
    int m_offset = 0;

    QString m_logText;
};

#endif // CONTROLLER_H


