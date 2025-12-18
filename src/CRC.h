#pragma once
#include <QByteArray>

class CRC8 {
public:
    static unsigned char CRC_calc(const unsigned char *buffer, unsigned int len);

    static unsigned char CRC_calc(const QByteArray &data);
};
