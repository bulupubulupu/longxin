#include <stdio.h>
#include "./../include/rs485.h"
int RS485_Init(struct ls2k500_uart *usart)
{
    int res = 0;
    res = USART_Init(usart);
    if(res != 0)
    {
        return -1;
    }
    res = read_thread_init(usart);
    if(res != 0)
    {
        return -1;
    }
    return 0;
}

// CRC16计算函数
uint16_t calculateCRC16(const uint8_t *data, uint16_t length) {
    uint16_t crc = 0xFFFF;
    for (int pos = 0; pos < length; pos++) {
        crc ^= data[pos];
        for (int i = 8; i != 0; i--) { // 处理每一位
            if ((crc & 0x0001) != 0) {
                crc >>= 1;
                crc ^= 0xA001; // CRC16的多项式
            } else { // 低位为0
                crc >>= 1;
            }
        }
    }
    return crc;
}


int RS485_read(struct ls2k500_uart *tUsart, char *buf)
{
    return usart_read(tUsart, buf);
}

int RS485_Write(struct ls2k500_uart *tUsart, char *buf, int len)
{
    return usart_write(tUsart, buf, len);
}