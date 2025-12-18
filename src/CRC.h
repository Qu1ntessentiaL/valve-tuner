/**
 * @file CRC.h
 * @brief Утилита для вычисления 8-битной CRC поверх кадров протокола.
 */

#pragma once
#include <QByteArray>

/**
 * @brief Вспомогательный класс для расчёта 8-битной контрольной суммы CRC.
 *
 * Реализация использует начальное значение @c 0xDE и полином @c 0x8C,
 * которые соответствуют протоколу устройства (совместимость со старой реализацией).
 */
class CRC8 {
public:
    /**
     * @brief Вычисляет CRC8 для произвольного буфера байт.
     * @param buffer Указатель на начало буфера.
     * @param len    Количество байт, участвующих в расчёте.
     * @return Вычисленное значение CRC8.
     */
    static unsigned char CRC_calc(const unsigned char *buffer, unsigned int len);

    /**
     * @brief Удобная перегрузка для @ref QByteArray.
     * @param data Входные данные.
     * @return Вычисленное значение CRC8.
     */
    static unsigned char CRC_calc(const QByteArray &data);
};
