#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "./../include/WiFi.h"

char ESP8266_Broadcast[40] = "ssid:esp_xp_lz,password:123456789";  /* 初始化指令 */
unsigned char ESP8266_Buffer_Data[40] = {0};
struct ls2k500_uart *ESP8266_Usart = NULL;

int ESP8266_Init(struct ls2k500_uart *tUsart)
{
    int len = 0;
    usart_write(tUsart, ESP8266_Broadcast, sizeof(ESP8266_Broadcast));
    usleep(300000);
    usart_write(tUsart, "start:wifi up", 13);
    memcpy(&ESP8266_Usart, &tUsart, sizeof(struct ls2k500_uart));
    return -1;
}
void ESP8266_Read(int command)
{ 
    usart_write(ESP8266_Usart, (char*)ESP8266_Buffer_Data, 8);
    memset(ESP8266_Buffer_Data, 0 , sizeof(ESP8266_Buffer_Data));
    // if(0 == RS485_read(ESP8266_Usart, (char*)ESP8266_Buffer_Data))
    // {
    //     printf("PH_3SUM1_Read_ERROR!");
    //     return;
    // }
    ESP8266_Analy(ESP8266_Buffer_Data, command);
    return;
}
int ESP8266_Write(char *buf, int len)
{
    memcpy(ESP8266_Buffer_Data, buf, len);
    usart_write(ESP8266_Usart,ESP8266_Buffer_Data,len);
    memset(ESP8266_Buffer_Data, 0 , sizeof(ESP8266_Buffer_Data));
    return;
}


int ESP8266_Analy(unsigned char *data, int command)
{
    return -1;
}
