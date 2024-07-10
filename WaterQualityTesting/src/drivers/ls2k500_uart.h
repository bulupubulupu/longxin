
#ifndef _LS2K500_UART_H_
#define _LS2K500_UART_H_

#include <stdint.h>

#define UART0 "ttyS0"
#define UART1 "ttyS1"
#define UART2 "ttyS2"
#define UART3 "ttyS3"
#define UART5 "ttyS5"
#define UART7 "ttyS7"
#define UART9 "ttyS9"

#define UART0_PATH "/dev/" UART0
#define UART1_PATH "/dev/" UART1
#define UART2_PATH "/dev/" UART2
#define UART3_PATH "/dev/" UART3
#define UART5_PATH "/dev/" UART5
#define UART7_PATH "/dev/" UART7
#define UART9_PATH "/dev/" UART9

#define BUFFER_SIZE 1024


extern int wifi_send_flag;
extern int phs_data_ls;
struct ls2k500_uart
{
    const char *USART_Path;
    uint32_t USART_BaudRate;    //波特率
    uint16_t USART_WordLength;  //收发数据位
    uint16_t USART_StopBits;    //停止位
    uint16_t USART_Parity;      //奇偶效验模式
    uint16_t USART_Mode;        //接受发送模式
    
    int fd;
};

int USART_Init(struct ls2k500_uart *tUsart);
int read_thread_init(struct ls2k500_uart *tUsart);

int usart_read(struct ls2k500_uart *tUsart, char *buf);
int usart_write(struct ls2k500_uart *tUsart, char *buf, int len);
void Usart_IQR(struct ls2k500_uart *tUsart);

#endif
