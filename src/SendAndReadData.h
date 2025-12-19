/**
 * @file SendandReadData.h
 * @brief Высокоуровневая обёртка над UART с кадрированием протокола и CRC.
 */

#pragma once

#include "USART.h"

/// Глобальный указатель на UART, используемый классами @ref Data и @ref Controller.
static UART *uart = nullptr;

/**
 * @brief Транспортный класс, реализующий кадрированный протокол устройства.
 *
 * Наследует @ref UART и добавляет:
 *  - формирование кадров (FEND/FESC, экранирование служебных байт),
 *  - расчёт контрольной суммы CRC8,
 *  - удобные методы @ref SendData и @ref RecieveData для кода алгоритма.
 */
class Data : public UART {
    /// Специальные байты, используемые в кадрирующем протоколе.
    enum {
        FEND = 0xC0, ///< Frame End.
        FESC = 0xDB, ///< Frame Escape.
        TFEND = 0xDC, ///< Transposed Frame End.
        TFESC = 0xDD, ///< Transposed Frame Escape.
    };

    static unsigned char CRC_calc(const unsigned char *buffer, unsigned int len) {
        unsigned char crc = 0xDE;
        while (len--) {
            crc ^= *buffer++;

            for (uint8_t i = 0; i < 8; i++)
                crc = (crc & 1) ? (crc >> 1) ^ 0x8c : crc >> 1;
        }
        return crc;
    }

    static unsigned char CRC_calc(const QByteArray &data) {
        return CRC_calc(reinterpret_cast<const unsigned char *>(data.constData()),
                        static_cast<unsigned int>(data.size()));
    }

public:
    /**
     * @brief Распарсенный ответ от устройства в удобном виде.
     *
     * @var DataNode::tag
     *   Идентификатор тега/команды протокола.
     * @var DataNode::data
     *   Числовое значение полезной нагрузки (уже объединённое из двух байт).
     */
    struct DataNode {
        char tag;
        double data;
    };

    /// Конструирует транспорт данных, используя заданное имя COM-порта.
    explicit Data(const QString &Portname) : UART(Portname) {}

    /**
     * @brief Отправляет закодированную команду с двумя одно байтовыми полями данных.
     *
     * Функция формирует кадр протокола: добавляет заголовок, считает CRC8,
     * выполняет байт-стаффинг служебных байт, а затем передаёт результат
     * в @ref UART::transmitUART().
     *
     * @param address Адрес устройства.
     * @param command Код команды.
     * @param data1   Первый байт данных.
     * @param data2   Второй байт данных.
     * @param fend    Маркер начала/конца кадра (по умолчанию @c FEND).
     */
    void SendData(unsigned char address,
                  unsigned char command,
                  unsigned char data1,
                  unsigned char data2,
                  unsigned char fend = FEND) {
        QByteArray data;
        data.append(fend);
        data.append(address);
        data.append(command);
        data.append(data1);
        data.append(data2);

        QByteArray result;
        unsigned char crcValue = CRC_calc(data);

        QByteArray dataWithCRC = data;
        dataWithCRC.append(crcValue);

        result.append(dataWithCRC[0]);

        for (int i = 1; i < dataWithCRC.size(); i++) {
            unsigned char currentByte = dataWithCRC[i];

            if (currentByte == FEND) {
                result.append(FESC);
                result.append(TFEND);
            } else if (currentByte == FESC) {
                result.append(FESC);
                result.append(TFESC);
            } else {
                result.append(dataWithCRC[i]);
            }
        }

        uart->transmitUART(result);
    }

    /**
     * @brief Отправляет команду с 16-битным значением данных.
     *
     * Значение разбивается на два байта и передаётся во второй вариант SendData().
     *
     * @param address Адрес устройства.
     * @param command Код команды.
     * @param data    16-битное значение данных.
     */
    void SendData(unsigned char address, unsigned char command, uint16_t data) {
        SendData(address, command, (data & 0xFF), (data & 0xff00) >> 8);
    }

    /**
     * @brief Принимает и декодирует один кадр ответа от устройства.
     *
     * Метод выполняет обратный байт-стаффинг и извлекает тег и 16-битное
     * значение в структуру @ref DataNode.
     *
     * @param node Указатель на структуру, в которую будет помещён результат.
     */
    void RecieveData(DataNode *node) {
        QByteArray data;
        data = uart->recieveUART();

        QByteArray result;
        result.append(data[0]);

        for (int i = 1; i < data.size(); i++) {
            // de-byte stuffing
            unsigned char currentByte = data[i];
            if (currentByte == FESC) {
                continue;
            } else if (currentByte == TFEND) {
                result.append(FEND);
            } else if (currentByte == TFESC) {
                result.append(FESC);
            } else {
                result.append(currentByte);
            }
        }

        // TODO: check CRC before accepting the frame.

        node->tag = (result[2]);
        node->data = static_cast<uint16_t>(static_cast<uint8_t>(result[3])) |
                     (static_cast<uint16_t>(static_cast<uint8_t>(result[4])) << 8);
    }
};
