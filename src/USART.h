/**
 * @file USART.h
 * @brief Тонкая обёртка над QSerialPort с простым логированием.
 */

#pragma once

#include <QSerialPort>
#include <QElapsedTimer>
#include <QThread>
#include <QDir>
#include <QDateTime>

/**
 * @brief Низкоуровневая обёртка UART на базе QSerialPort.
 *
 * Предоставляет базовые операции открытия/закрытия порта, блокирующей
 * передачи/приёма фиксированного пакета и примитивное логирование
 * в текстовый файл.
 */
class UART {
    QSerialPort serialPort;

public:
    /**
     * @brief Конструктор UART, привязанный к указанному последовательному порту.
     * @param Portname Имя порта (например, "COM3").
     * @param BaudRate Скорость обмена (по умолчанию 115200 бод).
     */
    UART(const QString &Portname, qint32 BaudRate = QSerialPort::Baud115200) {
        serialPort.setPortName(Portname);
        serialPort.setBaudRate(BaudRate);
        serialPort.setFlowControl(QSerialPort::NoFlowControl);
        serialPort.setDataBits(QSerialPort::Data8);
        serialPort.setRequestToSend(true);
        serialPort.setDataTerminalReady(true);
    };

    /**
     * @brief Открывает последовательный порт.
     * @param mode Режим открытия Qt (чтение/запись или оба).
     * @return @c true при успешном открытии, @c false при ошибке.
     */
    bool initUART(QSerialPort::OpenMode mode = QIODevice::ReadWrite) {
        return serialPort.open(mode);
    }

    /**
     * @brief Передаёт массив байт в последовательный порт и логирует его.
     * @param data Буфер, который необходимо отправить.
     */
    void transmitUART(QByteArray &data) {
        serialPort.write(data);
        serialPort.waitForBytesWritten(100);
        logUARTData("WRITING", data);
    }

    /**
     * @brief Принимает один пакет фиксированного размера из порта.
     *
     * Читает данные до PACKET_SIZE байт или до истечения тайм-аута,
     * логирует всё, что прочитано. Если первый байт кадра равен 0xC0,
     * возвращает первые PACKET_SIZE байт, иначе возвращает пустой массив.
     */
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

    /**
     * @brief Закрывает последовательный порт.
     */
    void closeUART() {
        serialPort.close();
    }

    /**
     * @brief Добавляет одну строку в файл @c debug.log с временной меткой.
     * @param message Текст сообщения для записи.
     */
    static void logToFile(const QString &message) {
        QFile file("debug.log");

        if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
            QTextStream stream(&file);
            QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
            stream << timestamp << " - " << message << "\n";
            file.close();
        }
    }

    /**
     * @brief Логирует буфер UART-данных с заданным префиксом.
     * @param prefix Текстовый префикс, описывающий контекст (например, WRITING/READING).
     * @param data   Сырые байты, которые нужно занести в лог.
     */
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

    /**
     * @brief Логирует вещественное значение с префиксом.
     * @param prefix Текстовый префикс, описывающий контекст.
     * @param value  Числовое значение для логирования.
     */
    static void logUARTData(const QString &prefix, double value) {
        QString message = QString("%1: %2").arg(prefix).arg(value, 0, 'f', 2);
        logToFile(message);
    }
};
