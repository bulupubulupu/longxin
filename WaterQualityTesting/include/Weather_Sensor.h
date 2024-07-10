#ifndef WEATHER_SENSOR_H_
#define WEATHER_SENSOR_H_

#include "./../src/drivers/ls2k500_uart.h"

// #define DEVICE_PROTOCOL_1   4       //海拔高度
// #define DEVICE_PROTOCOL_2   6       //盐度(PSU)
// #define DEVICE_PROTOCOL_3   10      //设备地址
// #define DEVICE_PROTOCOL_4   12      //上电自动测量(上电后进入在线模式)
// #define DEVICE_PROTOCOL_5   14      //溶氧校准操作
// #define DEVICE_PROTOCOL_6   16      //温度
// #define DEVICE_PROTOCOL_7   18      //溶氧比
// #define DEVICE_PROTOCOL_8   20      //溶氧值
// #define DEVICE_PROTOCOL_9   22      //状态位

// #define DEVICE_PROTOCOL_10  24      //软件版本
// #define DEVICE_PROTOCOL_11  26      //硬件版本
// #define DEVICE_PROTOCOL_12  28      //出厂版本
// #define DEVICE_PROTOCOL_13  30      //传感器工作时长
// #define DEVICE_PROTOCOL_14  32      //SN

// #define DEVICE_PROTOCOL_15  38      //测量PH值
// #define DEVICE_PROTOCOL_16  40      //PH校准操作
// #define DEVICE_PROTOCOL_17  42      //恢复出厂设置

// #define DEVICE_PROTOCOL_18  4608    //产品信息
// #define DEVICE_PROTOCOL_19  4610    //版本信息
// #define DEVICE_PROTOCOL_20  4620    //激活码
// #define DEVICE_PROTOCOL_21  4624    //起始工作时间

int Weather_Sensor_Init(struct ls2k500_uart *tUsart);
void Weather_Sensor_Read(int command);
int Weather_Sensor_Write(int command, char *buf, int len);

#endif