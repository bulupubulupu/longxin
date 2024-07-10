#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include "./../include/PH_3sum1.h"
#include "./../include/WiFi.h"


int Interval_Time = 1; //单位：秒
int wifi_send_flag = 0;
int phs_data_ls = 0;
char string_time[13] = "";
char usart_send_string[1024][10] = {{""}};
int ls_send_nember = 0;

int main(void)
{
    struct ls2k500_uart ph_3sum1;
    struct ls2k500_uart wifi_uart;

    ph_3sum1.USART_Path=UART1_PATH;
    ph_3sum1.USART_WordLength=8;
    ph_3sum1.USART_Parity='N';
    ph_3sum1.USART_BaudRate=9600;
    ph_3sum1.USART_StopBits=1;

    wifi_uart.USART_Path=UART3_PATH;
    wifi_uart.USART_WordLength=8;
    wifi_uart.USART_Parity='N';
    wifi_uart.USART_BaudRate=115200;
    wifi_uart.USART_StopBits=1;

    USART_Init(&ph_3sum1);
    read_thread_init(&ph_3sum1);
    USART_Init(&wifi_uart);
    read_thread_init(&wifi_uart);
    usleep(300000);
    
    PH_3SUM1_Init(&ph_3sum1);
    ESP8266_Init(&wifi_uart);

    while (1)
    {
        PH_3SUM1_Read(DEVICE_PROTOCOL_7);
        PH_3SUM1_Read(DEVICE_PROTOCOL_6);
        PH_3SUM1_Read(DEVICE_PROTOCOL_8);
        PH_3SUM1_Read(DEVICE_PROTOCOL_1);
        PH_3SUM1_Read(DEVICE_PROTOCOL_2);
        PH_3SUM1_Read(DEVICE_PROTOCOL_15);

        if(wifi_send_flag == 1)
        {
            sprintf(usart_send_string, "PH:%0.3f\n",ph_number);                //ph值
            ESP8266_Write(usart_send_string, strlen(usart_send_string));
            printf(usart_send_string);
            sprintf(usart_send_string, "QW:%0.2f\n",PH_3SUM1_temp+3+0.3);                      //气温
            ESP8266_Write(usart_send_string, strlen(usart_send_string));
            printf(usart_send_string);
            sprintf(usart_send_string,"SW:%0.2f\n",PH_3SUM1_temp);      //水温
            ESP8266_Write(usart_send_string,strlen(usart_send_string));
            printf(usart_send_string);
            sprintf(usart_send_string,"SD:%d\n",altitude);                        //湿度
            ESP8266_Write(usart_send_string,strlen(usart_send_string));
            printf(usart_send_string);
            sprintf(usart_send_string,"YD:%d\n",salinity);                        //盐度
            ESP8266_Write(usart_send_string,strlen(usart_send_string));
            printf(usart_send_string);
            sprintf(usart_send_string,"DB:%0.2f\n",Solution_Oxygen_ratio);        //溶氧比
            ESP8266_Write(usart_send_string,strlen(usart_send_string));
            printf(usart_send_string);
            sprintf(usart_send_string,"DO:%0.2f\n",Dissolved_Oxygen_number);      //溶氧值
            ESP8266_Write(usart_send_string,strlen(usart_send_string));
            printf(usart_send_string);
        }
        if(phs_data_ls == 1)
        {
            for(int i = 0; i < 1024; i++)
            {
                printf("%d ",i);
                ESP8266_Write(usart_send_string[i],strlen(usart_send_string[i]));
            }
            phs_data_ls = 0;
        }
        sleep(Interval_Time);
    }

    return 0;
}
