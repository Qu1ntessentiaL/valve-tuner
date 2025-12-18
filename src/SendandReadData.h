#ifndef SENDANDREADDATA_H
#define SENDANDREADDATA_H
#include "USART.h"
#include "CRC.h"

static UART* uart = nullptr;

class Data : public UART
{
    CRC8 crc;

    enum
    {
        FEND  = 0xC0, // Frame End
        FESC  = 0xDB, // Frame Escape
        TFEND = 0xDC, // Transposed Frame End
        TFESC = 0xDD, // Transposed Frame Escape
    };

public:

    struct DataNode {
        char tag;
        double data;
    };

    explicit Data(const QString &Portname) : UART(Portname){

    }

 void SendData(unsigned char address, unsigned char command, unsigned char data1, unsigned char data2, unsigned char fend = FEND){
    QByteArray data;
    data.append(fend);
    data.append(address);
    data.append(command);
    data.append(data1);
    data.append(data2);

    QByteArray result;
    unsigned char crcValue = crc.CRC_calc(data);

    QByteArray dataWithCRC = data;
    dataWithCRC.append(crcValue);

    result.append(dataWithCRC[0]);

    for(int i = 1; i < dataWithCRC.size(); i++){
        unsigned char currentByte = dataWithCRC[i];

        if(currentByte == FEND){
            result.append(FESC);
            result.append(TFEND);
        }else if(currentByte == FESC){
            result.append(FESC);
            result.append(TFESC);
        }else{
            result.append(dataWithCRC[i]);
        }
    }

    uart->transmitUART(result);
}

 void SendData(unsigned char address, unsigned char command, uint16_t data){
     SendData(address, command, (data & 0xFF), (data & 0xff00) >> 8);
 }

void RecieveData(DataNode *node){
    QByteArray data;
    data = uart->recieveUART();

    QByteArray result;
    result.append(data[0]);

     for(int i = 1; i < data.size(); i++){   //de-byte stuffing
         unsigned char currentByte = data[i];
         if(currentByte == FESC){
             continue;
         }else if(currentByte == TFEND){
             result.append(FEND);
         }else if(currentByte == TFESC){
             result.append(FESC);
         }else {
             result.append(currentByte);
         }
     }

     //TODO: check crc

     node->tag = (result[2]);
     node->data = static_cast<uint16_t>(static_cast<uint8_t>(result[3])) |
                 (static_cast<uint16_t>(static_cast<uint8_t>(result[4])) << 8);
 }

};

#endif // SENDANDREADDATA_H
