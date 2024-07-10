#ifndef ESP8266_H_
#define ESP8266_H_

#include "./../src/drivers/ls2k500_uart.h"

extern int wifi_send_flag;

int ESP8266_Init(struct ls2k500_uart *tUsart);
void ESP8266_Read(int command);
int ESP8266_Write(char *buf, int len);
int ESP8266_Analy(unsigned char *data, int command);

#endif
