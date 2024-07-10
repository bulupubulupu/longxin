#include <stdio.h>
#include <string.h>
#include <math.h>
#include "./../include/rs485.h"
#include "./../include/PH_3sum1.h"

struct ls2k500_uart *Weather_Sensor_Usart = NULL;
unsigned char Weather_Sensor_Buffer_Data[40] = {0};

unsigned char Device_Address = 0;
unsigned char Device_Type = 0;

/* 调用此函数之前应先调用串口初始化函数和接受线程函数 */
int Weather_Sensor_Init(struct ls2k500_uart *tUsart)
{
    //初始化接口函数 
    //
    int len = 0;
    RS485_Write(tUsart, (char *)Weather_Sensor_Buffer_Data, sizeof(Weather_Sensor_Buffer_Data));
    len = RS485_read(tUsart, Weather_Sensor_Usart);
    if(len == -1)
    {
        return -1; 
    }
    if(Weather_Sensor_Buffer_Data[0] == 0xFF && Weather_Sensor_Buffer_Data[1] == 0x03 && Weather_Sensor_Buffer_Data[2] == 0x04
    && Weather_Sensor_Buffer_Data[7] == 0xB4 && Weather_Sensor_Buffer_Data[8] == 0x33)
    {
        printf("PH_3SUM1_Init OK!\n");
        memcpy(&Weather_Sensor_Usart, &tUsart, sizeof(struct ls2k500_uart));
        Device_Address = Weather_Sensor_Buffer_Data[4];
        Device_Type = Weather_Sensor_Buffer_Data[6];

        return 0;
    }
    return -1;
}
