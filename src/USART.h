#pragma once

#include <QSerialPort>
#include <QElapsedTimer>
#include <QThread>
#include <QDir>
#include <QDateTime>

class UART {
    QSerialPort serialPort;

public:
    UART(const QString &Portname, qint32 BaudRate = QSerialPort::Baud115200) {
        serialPort.setPortName(Portname);
        serialPort.setBaudRate(BaudRate);
        serialPort.setFlowControl(QSerialPort::NoFlowControl);
        serialPort.setDataBits(QSerialPort::Data8);
        serialPort.setRequestToSend(true);
        serialPort.setDataTerminalReady(true);
    };

    bool initUART(QSerialPort::OpenMode mode = QIODevice::ReadWrite) {
        return serialPort.open(mode);
    }

    void transmitUART(QByteArray &data) {
        serialPort.write(data);
        serialPort.waitForBytesWritten(100);
        logUARTData("WRITING", data);
    }

    QByteArray recieveUART() {
        QByteArray data;
        const int PACKET_SIZE = 6;
        const int MAX_WAIT_MS = 1000;
        // serialPort.clear();

        QElapsedTimer timer;
        timer.start();

        while (timer.elapsed() < MAX_WAIT_MS && data.size() < PACKET_SIZE) {
            if (serialPort.waitForReadyRead(1000)) {
                data.append(serialPort.readAll());
                //data = serialPort.readAll();
                logUARTData("READING", data);
            }
            QThread::msleep(1);
        }
        if (data.size() >= PACKET_SIZE && static_cast<quint8>(data[0]) == 0xC0) {
            return data.left(PACKET_SIZE);
        }
        return QByteArray();
    }

    void closeUART() {
        serialPort.close();
    }

    static void logToFile(const QString &message) {
        QFile file("debug.log");

        if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
            QTextStream stream(&file);
            QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
            stream << timestamp << " - " << message << "\n";
            file.close();
        }
    }

    // ??????? 1: ??????? ??????????? ??????? ? QByteArray
    static void logUARTData(const QString &prefix, const QByteArray &data) {
        if (data.isEmpty()) {
            logToFile(prefix + ": no data");
            return;
        }

        QString message = QString("%1: %2 byte - %3")
                .arg(prefix)
                .arg(data.size())
                .arg(QString(data.toHex()));

        logToFile(message);
    }

    static void logUARTData(const QString &prefix, double value) {
        QString message = QString("%1: %2").arg(prefix).arg(value, 0, 'f', 2);
        logToFile(message);
    }
};
