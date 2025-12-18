#include "Controller.h"

#include <QDebug>

Controller::Controller(QObject *parent)
    : QObject(parent)
{
    m_timer.setInterval(100);
    connect(&m_timer, &QTimer::timeout, this, &Controller::updateInsufflatorData);
}

void Controller::setPortName(const QString &name)
{
    if (m_portName == name)
        return;
    m_portName = name;
    emit portNameChanged();
}

void Controller::appendLog(const QString &line)
{
    if (!m_logText.isEmpty())
        m_logText.append('\n');
    m_logText.append(line);
    emit logTextChanged();
}

void Controller::resetMeasurement()
{
    m_inValue = insufflator::INValue{};
    m_flowTarget = 2.0;
    m_slope = 0.0;
    m_offset = 0;
    emit calibrationChanged();
    emit resultChanged();
}

void Controller::connectOrDisconnect()
{
    if (!m_connected) {
        if (m_portName.isEmpty()) {
            emit errorOccurred(tr("Port name is empty"));
            return;
        }

        // global uart lives in this translation unit (see SendandReadData.h)
        uart = new UART(m_portName);
        if (!uart->initUART()) {
            delete uart;
            uart = nullptr;
            emit errorOccurred(tr("Could not connect to the port"));
            return;
        }

        m_data = new Data(m_portName);

        m_connected = true;
        appendLog(tr("Connected to %1").arg(m_portName));
        emit connectedChanged();
    } else {
        m_timer.stop();
        m_running = false;
        emit runningChanged();

        delete m_in;
        m_in = nullptr;

        delete m_data;
        m_data = nullptr;

        if (uart) {
            uart->closeUART();
            delete uart;
            uart = nullptr;
        }

        m_connected = false;
        appendLog(tr("Disconnected"));
        emit connectedChanged();
    }
}

void Controller::startOrStop()
{
    if (!m_connected) {
        emit errorOccurred(tr("Connect to the device first"));
        return;
    }

    if (!m_running) {
        resetMeasurement();
        delete m_in;
        m_in = nullptr;

        m_running = true;
        emit runningChanged();
        m_timer.start();
    } else {
        m_timer.stop();
        m_running = false;
        emit runningChanged();

        delete m_in;
        m_in = nullptr;
    }
}

void Controller::updateInsufflatorData()
{
    if (!m_data)
        return;

    if (m_flowTarget == 2.0) {
        if (m_in != nullptr) {
            m_in->algorithms();
            m_pwm = m_in->pwm;
            m_flow = m_in->currentFlow;
            m_error = m_in->error;
            emit valuesChanged();

            if (std::fabs(m_in->error) < 0.3) {
                m_inValue.PWM1 = m_pwm;
                m_inValue.FLOW1 = m_flow;
                m_flowTarget = 20.0;
                appendLog(tr("Point 1: PWM=%1, FLOW=%2")
                              .arg(m_inValue.PWM1)
                              .arg(m_inValue.FLOW1, 0, 'f', 3));
                delete m_in;
                m_in = nullptr;
                emit calibrationChanged();
            }
        } else {
            m_in = new insufflator(m_data, m_flowTarget);
        }
    } else if (m_flowTarget == 20.0) {
        if (m_in != nullptr) {
            m_in->algorithms();
            m_pwm = m_in->pwm;
            m_flow = m_in->currentFlow;
            m_error = m_in->error;
            emit valuesChanged();

            if (std::fabs(m_in->error) < 0.3) {
                m_inValue.PWM2 = m_pwm;
                m_inValue.FLOW2 = m_flow;
                m_flowTarget = 0.0;
                appendLog(tr("Point 2: PWM=%1, FLOW=%2")
                              .arg(m_inValue.PWM2)
                              .arg(m_inValue.FLOW2, 0, 'f', 3));
                delete m_in;
                m_in = nullptr;
                emit calibrationChanged();
            }
        } else {
            m_in = new insufflator(m_data, m_flowTarget);
        }
    } else if ((m_inValue.PWM1 > 0) && (m_inValue.PWM2 > 0) &&
               (m_inValue.FLOW1 > 0.0) && (m_inValue.FLOW2 > 0.0)) {
        auto res = insufflator::approximate(m_inValue);
        m_slope = res.slope;
        m_offset = res.offset;
        emit resultChanged();

        appendLog(tr("Approximation: slope=%1, offset=%2")
                      .arg(m_slope, 0, 'f', 2)
                      .arg(m_offset));

        m_timer.stop();
        m_running = false;
        emit runningChanged();
    }
}


