#ifndef PH_3SUM1_H_
#define PH_3SUM1_H_

#include "./../src/drivers/ls2k500_uart.h"

#define DEVICE_PROTOCOL_1   4       //海拔高度
#define DEVICE_PROTOCOL_2   6       //盐度(PSU)
#define DEVICE_PROTOCOL_3   10      //设备地址
#define DEVICE_PROTOCOL_4   12      //上电自动测量(上电后进入在线模式)
#define DEVICE_PROTOCOL_5   14      //溶氧校准操作
#define DEVICE_PROTOCOL_6   16      //温度
#define DEVICE_PROTOCOL_7   18      //溶氧比
#define DEVICE_PROTOCOL_8   20      //溶氧值
#define DEVICE_PROTOCOL_9   22      //状态位

#define DEVICE_PROTOCOL_10  24      //软件版本
#define DEVICE_PROTOCOL_11  26      //硬件版本
#define DEVICE_PROTOCOL_12  28      //出厂版本
#define DEVICE_PROTOCOL_13  30      //传感器工作时长
#define DEVICE_PROTOCOL_14  32      //SN

#define DEVICE_PROTOCOL_15  38      //测量PH值
#define DEVICE_PROTOCOL_16  40      //PH校准操作
#define DEVICE_PROTOCOL_17  42      //恢复出厂设置

#define DEVICE_PROTOCOL_18  4608    //产品信息
#define DEVICE_PROTOCOL_19  4610    //版本信息
#define DEVICE_PROTOCOL_20  4620    //激活码
#define DEVICE_PROTOCOL_21  4624    //起始工作时间

extern double PH_3SUM1_temp;       //温度存放
extern double Solution_Oxygen_ratio;//溶氧比存放
extern double Dissolved_Oxygen_number;//溶氧值存放
extern int altitude;               //海拔高度
extern int salinity;               //盐度
extern double ph_number;
extern int phs_data_ls;
extern char usart_send_string[1024][10];
extern int ls_send_nember;
int PH_3SUM1_Init(struct ls2k500_uart *tUsart);
void PH_3SUM1_Read(int command);
int PH_3SUM1_Write(int command, char *buf, int len);

#endif
