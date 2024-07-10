#ifndef __LS2KRS485_H_
#define __LS2KRS485_H_

#include "./../src/drivers/ls2k500_uart.h"

int RS485_Init(struct ls2k500_uart *usart);
int RS485_read(struct ls2k500_uart *tUsart, char *buf);
int RS485_Write(struct ls2k500_uart *tUsart, char *buf, int len);

uint16_t calculateCRC16(const uint8_t *data, uint16_t length);

#endif
