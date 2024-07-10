#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <termios.h>
#include <semaphore.h>
#include <strings.h>
#include <errno.h>
#include <string.h>
#include "./ls2k500_uart.h"

typedef struct {
    char buffer[BUFFER_SIZE];
    int size; // 当前缓冲区中的数据大小
    pthread_mutex_t mutex; // 互斥锁
    int data_available; // 条件变量
    int port_number; // 串口号
} SerialPortData;


// char *test_str[9] = {"12345", "11111", "22222", "33333", "44444" , "55555", "23456", "13579", "54321"};
#define NUM_PORTS 7
static SerialPortData USART_SEND_BUFFER_DATA[NUM_PORTS]; // 共享数据结构数组


int USART_Init(struct ls2k500_uart *tUsart)
{
    struct termios options, oldtio;
    if(tUsart->USART_Path == NULL)
    {
        printf("USART INIT ERROR!\n");
        return -1;
    }
    tUsart->fd = open(tUsart->USART_Path, O_RDWR|O_NOCTTY|O_NONBLOCK);
    if(tUsart->fd == -1)
    {
		printf("Can't Open Serial Prot!\n");
        return -1;
    }
    if(fcntl(tUsart->fd, F_SETFL, 0) < 0)
    {
        printf("fcntl failed!\n");
        return -1;
    }

	/*测试是否为终端设备*/

	if(isatty(STDIN_FILENO)==0)
		printf("standard input is not a terminal device\n");
	else
		printf("isatty success!\n");
    /*保存测试现有串口参数设置，在这里如果串口号等出错，会有相关的出错信息*/
    if(tcgetattr(tUsart->fd, &oldtio)!=0)
    {
        printf("Setup Serial %d", tUsart->fd);
        return -1;
    }

    bzero(&options, sizeof(options));

	/*本地连接 接受使能*/
	options.c_cflag  |=  CLOCAL | CREAD; 
	options.c_cflag &= ~CSIZE;
	switch(tUsart->USART_WordLength)
	{
		case 7:
			options.c_cflag |= CS7;
			break;
		
		case 8:
			options.c_cflag |= CS8;
			break;
		
		default:
			printf("Unsupported data size!\n");
			return -1;
			
	}
	switch(tUsart->USART_Parity)
	{
		case 'O': //奇数
			options.c_cflag |= PARENB;
	     	options.c_cflag |= PARODD;
	     	options.c_iflag |= (INPCK | ISTRIP);
	     	break;
			
		case 'E': //偶数
			options.c_iflag |= (INPCK | ISTRIP);
			options.c_cflag |= PARENB;
			options.c_cflag &= ~PARODD;
			break;

		case 'N':  //无奇偶校验位
			options.c_cflag &= ~PARENB;
			break;
		
		default:
			printf("Please Set USART Parity O/E/N !\n");
			return -1;
			
	}
	
	options.c_lflag  &= ~(ICANON | ECHO | ECHOE | ISIG);  /*Input*/
	options.c_oflag  &= ~OPOST;   /*Output*/

	switch (tUsart->USART_BaudRate)
	{
		case 2400:
			cfsetispeed(&options, B2400);
			cfsetospeed(&options, B2400);
			break;

		case 4800:
			cfsetispeed(&options, B4800);
			cfsetospeed(&options, B4800);
			break;

		case 9600:
			cfsetispeed(&options, B9600);
			cfsetospeed(&options, B9600);
			break;

		case 115200:
			cfsetispeed(&options, B115200);
			cfsetospeed(&options, B115200);
			break;

		case 460800:
			cfsetispeed(&options, B460800);
			cfsetospeed(&options, B460800);
			break;

		default:
			cfsetispeed(&options, B9600);
			cfsetospeed(&options, B9600);
			break;
			
	}

	switch (tUsart->USART_StopBits)
	{
		case 1:
			options.c_cflag &= ~CSTOPB;
			break;
		case 2:
			options.c_cflag |= CSTOPB;
			break;
		
	}
	options.c_cc[VMIN]  = 1; /* 读数据时的最小字节数: 没读到这些数据我就不返回! */
	options.c_cc[VTIME] = 1; /* 等待第1个数据的时间: 
	                         * 比如VMIN设为10表示至少读到10个数据才返回,
	                         * 但是没有数据总不能一直等吧? 可以设置VTIME(单位是10秒)
	                         * 假设VTIME=1，表示: 
	                         *    10秒内一个数据都没有的话就返回
	                         *    如果10秒内至少读到了1个字节，那就继续等待，完全读到VMIN个数据再返回
	                         */
	/*处理未接收字符*/
    tcflush(tUsart->fd,TCIFLUSH);
	/*激活新配置*/
	if((tcsetattr(tUsart->fd,TCSANOW,&options))!=0)
	{
		printf("com set error\n");
		return -1;
	}
	
	return 0;

}

int serial_number_back(struct ls2k500_uart *tUsart)
{
	if(!strncmp(tUsart->USART_Path, UART0_PATH,strlen(UART0_PATH)))
	{
		return 0;
	} else if (!strncmp(tUsart->USART_Path, UART1_PATH,strlen(UART1_PATH)))
	{
		return 1;
	}else if (!strncmp(tUsart->USART_Path, UART2_PATH,strlen(UART2_PATH)))
	{
		return 2;
	}else if (!strncmp(tUsart->USART_Path, UART3_PATH,strlen(UART3_PATH)))
	{
		return 3;
	} else if (!strncmp(tUsart->USART_Path, UART5_PATH,strlen(UART5_PATH)))
	{
		return 4;
	} else if (!strncmp(tUsart->USART_Path, UART7_PATH,strlen(UART7_PATH)))
	{
		return 5;
	} else if (!strncmp(tUsart->USART_Path, UART9_PATH,strlen(UART9_PATH)))
	{
		return 6;
	}
	return -1;
}


/* 串口接受线程 
 * BUG：Linux缓冲区数据大小在8字节
 */
void *serial_read_thread(void *arg)
{
	struct ls2k500_uart *tUsart = (struct ls2k500_uart *)arg;
	char buffer[32] = {0};
	int nbyte, len = 0;
	int res = 0;
	int SerialPort_int = 0;
	SerialPort_int = serial_number_back(tUsart);
	printf("SerialPort = %d\n",SerialPort_int);
	memset(USART_SEND_BUFFER_DATA[SerialPort_int].buffer, 0, BUFFER_SIZE);
	pthread_mutex_init(&USART_SEND_BUFFER_DATA[SerialPort_int].mutex, NULL);
	USART_SEND_BUFFER_DATA[SerialPort_int].port_number = SerialPort_int;
	USART_SEND_BUFFER_DATA[SerialPort_int].data_available = 0;
	// res = sem_init(&USART_SEND_BUFFER_DATA[SerialPort_int].data_available, 0, 0);
	// if (res != 0)
	// {
	// 	perror("Semaphore initialization failed");
	// }
	// printf("sem_init\n");
	while(1)
	{
		while((nbyte = read(tUsart->fd, buffer, 32)) > 0)
		{
			if(nbyte == 8)
			{
				memmove(USART_SEND_BUFFER_DATA[SerialPort_int].buffer + len, buffer, nbyte);
				len += nbyte;
			} else if(nbyte >0 || nbyte < 8)
			{
				pthread_mutex_lock(&USART_SEND_BUFFER_DATA[SerialPort_int].mutex);
				// strcat(USART_SEND_BUFFER_DATA[SerialPort_int].buffer, buffer);
				memmove(USART_SEND_BUFFER_DATA[SerialPort_int].buffer + len, buffer, nbyte);
				len += nbyte;
				USART_SEND_BUFFER_DATA[SerialPort_int].size = len;len = 0;
				pthread_mutex_unlock(&USART_SEND_BUFFER_DATA[SerialPort_int].mutex);
				// sem_post(&USART_SEND_BUFFER_DATA[SerialPort_int].data_available);
				USART_SEND_BUFFER_DATA[SerialPort_int].data_available = 1;
				Usart_IQR(tUsart);
			}
		}
		if(nbyte < 0)
		{
			if(errno == EAGAIN)
			{
				continue;
			} else {
				printf("Read failed: %s\n", strerror(errno));
				close(tUsart->fd); // 关闭串口
                return NULL; // 退出线程
			}

		} else if(nbyte == 0)
		{
			printf("Serial port closed by remote.\n");
            close(tUsart->fd); // 关闭串口
            return NULL; // 退出线程
		} else 
		{
			// printf("read ok!\n");
			// printf("buf len = %d\n",nbyte);
			// pthread_mutex_lock(&USART_SEND_BUFFER_DATA[SerialPort_int].mutex);
			// USART_SEND_BUFFER_DATA[SerialPort_int].size = nbyte;
			// pthread_mutex_unlock(&USART_SEND_BUFFER_DATA[SerialPort_int].mutex);
			// sem_post(&(USART_SEND_BUFFER_DATA[SerialPort_int].data_available));
		}
	}
}



int read_thread_init(struct ls2k500_uart *tUsart)
{
	pthread_t tid;
	struct ls2k500_uart *tuart = (struct ls2k500_uart *)malloc(sizeof(struct ls2k500_uart));
	if (tuart == NULL)
    {
        perror("malloc failed");
        close(tUsart->fd); // 确保关闭打开的文件描述符
        return -1;
    }
	memcpy(tuart, tUsart, sizeof(struct ls2k500_uart));
	if(pthread_create(&tid, NULL, serial_read_thread, (void *)tuart) != 0)
	{
		perror("pthread_create failed");
		free(tuart);
		close(tuart->fd);
		return -1;
	}
	return 0;
}

int usart_read(struct ls2k500_uart *tUsart, char *buf)
{
	int8_t SerialPort_int = 0 ,overtime = 5;

	SerialPort_int = (int8_t)serial_number_back(tUsart);
	// sem_wait(&USART_SEND_BUFFER_DATA[SerialPort_int].data_available);
	/* 编写等待超时机制 */
	while(!USART_SEND_BUFFER_DATA[SerialPort_int].data_available)
	{
		if(overtime--)
		{
			usleep(100000);
		} else 
		{
			return -1;
		}
	}
	USART_SEND_BUFFER_DATA[SerialPort_int].data_available = 0;
	pthread_mutex_lock(&USART_SEND_BUFFER_DATA[SerialPort_int].mutex);
	memcpy(buf, USART_SEND_BUFFER_DATA[SerialPort_int].buffer, USART_SEND_BUFFER_DATA[SerialPort_int].size);
	bzero(USART_SEND_BUFFER_DATA[SerialPort_int].buffer, USART_SEND_BUFFER_DATA[SerialPort_int].size); //类似于memset
	pthread_mutex_unlock(&USART_SEND_BUFFER_DATA[SerialPort_int].mutex);

	return USART_SEND_BUFFER_DATA[SerialPort_int].size;
}

int usart_write(struct ls2k500_uart *tUsart, char *buf, int len)
{
	int num_bytes = 0;
	num_bytes = write(tUsart->fd, buf, len);
	if(num_bytes < 0)
	{
		printf("Error writing to USART: %s\n", strerror(errno));
		close(tUsart->fd);
		return -1;
	}
	return num_bytes;
}

void Usart_IQR(struct ls2k500_uart *tUsart)
{
    if(!strncmp(tUsart->USART_Path, UART3_PATH, strlen(UART3_PATH)))
    {
		printf(USART_SEND_BUFFER_DATA[3].buffer);
		if(!strncmp(USART_SEND_BUFFER_DATA[3].buffer, "start:send down", 15))
		{
			wifi_send_flag = 0;
		}
		if(!strncmp(USART_SEND_BUFFER_DATA[3].buffer, "start:send up", 15))
		{
			wifi_send_flag = 1;
		}
		if(!strncmp(USART_SEND_BUFFER_DATA[3].buffer, "PHS", 15))
		{
			// wifi_send_flag = 0;
			phs_data_ls = 1;
		}
		bzero(USART_SEND_BUFFER_DATA[3].buffer, 100);
    }
}