#include <stdio.h>
#include <string.h>
#include <math.h>
#include "./../include/rs485.h"
#include "./../include/PH_3sum1.h"
/* Modbus协议 */
/* 地址域 功能码 起始地址 寄存器地址 CRC */
unsigned char Inquire_Broadcast[8] = {0xFF, 0x03, 0x00, 0x0A, 0x00, 0x02, 0xF1, 0xD7};  /* 广播查询地址 */
unsigned char PH_3SUM1_Buffer_Data[40] = {0};
struct ls2k500_uart *PH_3SUM1_Usart = NULL;

unsigned char Device_Address = 0;
unsigned char Device_Type = 0;


double PH_3SUM1_temp = 0;       //温度存放
double Solution_Oxygen_ratio = 0;//溶氧比存放
double Dissolved_Oxygen_number = 0;//溶氧值存放
int altitude = 0;               //海拔高度
int salinity = 0;               //盐度
double ph_number = 0;
//状态标志类
uint8_t State_Dissolved_Oxygen_flag = 0;

//溶氧操作类
uint8_t DissolvedOxygen_Calibration_flag = 0;      //校验标志位
uint8_t DissolvedOxygen_0_Calibration_flag = 0;    //零氧校验有效标志位
uint8_t DissolvedOxygen_100_Calibration_flag = 0;  //饱和氧校验有效标志位
uint8_t DissolvedOxygen_Calibration_Time = 0;      //校验进行倒计时

/* 警告标志变量 */
uint8_t powerSupplyAndMebrane_flag = 0;
uint8_t Mebrane_lifeSpan_flag = 0;
uint8_t ratingData_flag = 0;

int Status_bit[5] = {0}; 


unsigned char reg0[3] = {0};
unsigned char reg1[3] = {0};
unsigned char CRCBuf[2]= {0};
unsigned int Valid_Data_Num = 0;


int PH_3SUM1_Analy_ModBus(unsigned char *data, int command)
{
    
    if(data[0] == Device_Address)
    {
        switch (command)
        {
        case DEVICE_PROTOCOL_1:
            if(data[1] == 0x03) //读操作
            {
                int EL_number = 0;
                memcpy(reg0, data + 3, 2);
                EL_number = reg0[0];
                EL_number = (EL_number << 8) + reg0[1];
                altitude = EL_number;

            } else if(data[1] == 0x10)//写操作
            {
                // memcpy(reg0, data + 2, 2);
                // memcpy(reg1, data + 3, 2);
                // if(command == (uint16_t)reg0)
                //     if(command == (uint16_t)reg1)
                //         return DEVICE_PROTOCOL_5;
            }
        break;
        case DEVICE_PROTOCOL_2:
            if(data[1] == 0x03) //读操作
            {
                int PSU_number = 0;
                memcpy(reg0, data + 3, 2);
                PSU_number = reg0[0];
                PSU_number = (PSU_number << 8) + reg0[1];
                salinity = PSU_number;


            } else if(data[1] == 0x10)//写操作
            {
                // memcpy(reg0, data + 2, 2);
                // memcpy(reg1, data + 3, 2);
                // if(command == (uint16_t)reg0)
                //     if(command == (uint16_t)reg1)
                //         return DEVICE_PROTOCOL_5;
            }
        break;
        case DEVICE_PROTOCOL_15:
        memcpy(reg0, data + 3, 2);
        int16_t ph_number_buf = 0;
        ph_number_buf = reg0[0];
        ph_number_buf = (ph_number_buf << 8) + reg0[1];
        ph_number = ph_number_buf;
        ph_number = ph_number/1000; 
        // if(phs_data_ls == 0)
        // {
            ls_send_nember++;
            if(ls_send_nember >= 1024)
            {
                ls_send_nember = 0;
            }
            
            sprintf(usart_send_string[ls_send_nember], "%d:%0.3f;", ls_send_nember, ph_number); 
        // }
        break;
        case DEVICE_PROTOCOL_6:
        memcpy(reg0, data + 3, 2);
        memcpy(reg1, data + 5, 2);
        int16_t temp = 0;
        int16_t temp_buf = 0;
        PH_3SUM1_temp = 0;
        temp = reg0[0];
        temp = (temp << 8) + reg0[1];
        temp -= 5000; 

        temp_buf = reg1[0];
        temp_buf = (temp_buf << 8) + reg1[1];
        if(temp - temp_buf)
        {
            temp = 0;
            temp_buf = 0;
            PH_3SUM1_temp = 25;
        } else {
            temp_buf = 0;
            PH_3SUM1_temp = temp;
            PH_3SUM1_temp = PH_3SUM1_temp/100;
        }
        break;
        case DEVICE_PROTOCOL_7:
        memcpy(reg0, data + 3, 2);
        // PH_3SUM1_Write(DEVICE_PROTOCOL_9);
        int Solution_ratio = 0;
        Solution_ratio = reg0[0];
        Solution_ratio = (Solution_ratio << 8) + reg0[1];
        //1：表示溶氧数据已更新  0：表示溶氧数据未更新，是上次测量数据
            Solution_Oxygen_ratio = (double)Solution_ratio/100;
            break;
        case DEVICE_PROTOCOL_8:
        // PH_3SUM1_Write(DEVICE_PROTOCOL_9);
        memcpy(reg0, data + 3, 2);
        int Solution_number = 0;
        Solution_number = reg0[0];
        Solution_number = (Solution_number << 8) + reg0[1];
        Dissolved_Oxygen_number = (double)Solution_number/ 100;
            break;
        case DEVICE_PROTOCOL_9:
        //7个状态标志
        memcpy(reg0, data + 3, 2);
        memcpy(reg1, data + 5, 2);
        uint16_t reg1_flag = (uint16_t)reg1;

        State_Dissolved_Oxygen_flag = reg1_flag & 0x0001;
        powerSupplyAndMebrane_flag = reg1_flag & 0x0002;
        Mebrane_lifeSpan_flag = reg1_flag & 0x0008;
        ratingData_flag = reg1_flag & 0x0020;
        /***********************************************/
        if(reg1_flag & 0x0001)
            State_Dissolved_Oxygen_flag = 1;
        else 
            State_Dissolved_Oxygen_flag = 0;
        if(reg1_flag & 0x0002)
            powerSupplyAndMebrane_flag = 1;
        else 
            powerSupplyAndMebrane_flag = 0;
        if(reg1_flag & 0x0008)
            Mebrane_lifeSpan_flag = 1;
        else 
            Mebrane_lifeSpan_flag = 0;
        if(reg1_flag & 0x0020)
            ratingData_flag = 1;
        else 
            ratingData_flag = 0;
        break;
        /***********************************************/
        default:
            break;
        }
    }
}

/* 调用此函数之前应先调用串口初始化函数和接受线程函数 */
int PH_3SUM1_Init(struct ls2k500_uart *tUsart)
{
    //初始化接口函数 
    //
    int len = 0;
    RS485_Write(tUsart, (char *)Inquire_Broadcast, sizeof(Inquire_Broadcast));
    len = RS485_read(tUsart, PH_3SUM1_Buffer_Data);
    if(len == -1)
    {
        return -1;
    }
    if(PH_3SUM1_Buffer_Data[0] == 0xFF && PH_3SUM1_Buffer_Data[1] == 0x03 && PH_3SUM1_Buffer_Data[2] == 0x04
    && PH_3SUM1_Buffer_Data[7] == 0xB4 && PH_3SUM1_Buffer_Data[8] == 0x33)
    {
        printf("PH_3SUM1_Init OK!\n");
        memcpy(&PH_3SUM1_Usart, &tUsart, sizeof(struct ls2k500_uart));
        Device_Address = PH_3SUM1_Buffer_Data[4];
        Device_Type = PH_3SUM1_Buffer_Data[6];

        return 0;
    }
    return -1;
}



void PH_3SUM1_Read(int command)
{
    PH_3SUM1_Buffer_Data[0] = Device_Address;
    PH_3SUM1_Buffer_Data[1] = 0x03;
    PH_3SUM1_Buffer_Data[2] = command/0xff;
    PH_3SUM1_Buffer_Data[3] = command%0xff;
    PH_3SUM1_Buffer_Data[4] = 0x00;
    PH_3SUM1_Buffer_Data[5] = 0x02;         //激活码命令除外
    // 5. CRC校验码
    uint16_t crc = calculateCRC16(PH_3SUM1_Buffer_Data, 6);
    PH_3SUM1_Buffer_Data[6] = (uint8_t)(crc & 0xFF); // CRC的低字节
    PH_3SUM1_Buffer_Data[7] = (uint8_t)(crc >> 8);   // CRC的高字节

    RS485_Write(PH_3SUM1_Usart, (char*)PH_3SUM1_Buffer_Data, 8);
    memset(PH_3SUM1_Buffer_Data, 0 , sizeof(PH_3SUM1_Buffer_Data));
    if(0 == RS485_read(PH_3SUM1_Usart, (char*)PH_3SUM1_Buffer_Data))
    {
        printf("PH_3SUM1_Read_ERROR!");
        return;
    }
    PH_3SUM1_Analy_ModBus(PH_3SUM1_Buffer_Data, command);
}

int PH_3SUM1_Write(int command, char *buf, int len)
{
    int i = 0, Difference = 0;
    if(len%2)
    {
        printf("PH_3SUM1_Write : The parameter setting is not a multiple of 2!\n");
        return -1;
    }
    Difference = len - strlen(buf);
    PH_3SUM1_Buffer_Data[0] = Device_Address;
    PH_3SUM1_Buffer_Data[1] = 0x10;
    PH_3SUM1_Buffer_Data[2] = command/0xff;
    PH_3SUM1_Buffer_Data[3] = command%0xff;
    PH_3SUM1_Buffer_Data[4] = (len/2)/0xff;
    PH_3SUM1_Buffer_Data[5] = (len/2)%0xff;
    PH_3SUM1_Buffer_Data[6] = len;
    for(i = 0; i < Difference; i++)
        PH_3SUM1_Buffer_Data[7 + i] = 0x00;
    for (i = 0; i < strlen(buf); i++)
    {   
        PH_3SUM1_Buffer_Data[7 + Difference + i] = buf[i];
    }

    // 5. CRC校验码
    uint16_t crc = calculateCRC16(PH_3SUM1_Buffer_Data, 7 + len);
    PH_3SUM1_Buffer_Data[7 + len] = (uint8_t)(crc & 0xFF); // CRC的低字节
    PH_3SUM1_Buffer_Data[7 + len + 1] = (uint8_t)(crc >> 8);   // CRC的高字节
    
    RS485_Write(PH_3SUM1_Usart,PH_3SUM1_Buffer_Data,7 + len + 2);
    memset(PH_3SUM1_Buffer_Data, 0 , sizeof(PH_3SUM1_Buffer_Data));
    if(0 == RS485_read(PH_3SUM1_Usart, PH_3SUM1_Buffer_Data))
    {
        printf("PH_3SUM1_Write_ERROR!");
        return;
    }

    PH_3SUM1_Analy_ModBus(PH_3SUM1_Buffer_Data, command);
}
