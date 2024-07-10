#include "../include/ls2k500_uart.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
int main(int argc, char **argv)
{
    struct ls2k500_uart usart1;

    if(argc == 3)
    {
        printf("Usage :%s <UART_PATH> <data>\n", argv[0]);
        return -1;
    }

    memset(&usart1,0,sizeof(usart1));
    usart1.USART_Path=UART1_PATH;
    usart1.USART_BaudRate=115200;
    usart1.USART_WordLength=8;
    usart1.USART_Parity='N';
    usart1.USART_StopBits=1;
    USART_Init(&usart1);
    if(read_thread_init(&usart1) != 0)
    {
        printf("ERROR: Read_thread_init Initialization error !\n");
        return -1;
    }
    
    while(1)
    {
        // usart_write(&usart1,"123");
        // sleep(100);
    }

    return 0;
}