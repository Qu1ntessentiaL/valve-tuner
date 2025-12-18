#include <CRC.h>


unsigned char CRC8::CRC_calc(const unsigned char *buffer, unsigned int len)
{
    unsigned char crc = 0xDE;
    unsigned int i;
    while (len--)
    {
        crc ^= *buffer++;

        for (i = 0; i < 8; i++)
            crc = (crc & 1)? (crc >> 1) ^ 0x8c : crc >> 1;
    }
    return crc;
}

unsigned char CRC8::CRC_calc(const QByteArray& data) {
    return CRC8::CRC_calc(reinterpret_cast<const unsigned char*>(data.constData()), static_cast<unsigned int>(data.size()));
}
