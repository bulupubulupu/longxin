
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include "ls2k500_i2c.h"

//初始化i2c
//i2c_dev设备文件 i2c_addr 设备地址 
//返回 0
int i2c_init(char *i2c_dev, unsigned char i2c_addr)
{
	int res = 0;
	int fd;
	
	fd = open(i2c_dev, O_RDWR);
	if(fd < 0)
	{
		printf("[%s]:[%d] open i2c file error\r\n", __FUNCTION__, __LINE__);
		return -1;
	}
	res = ioctl(fd,I2C_TENBIT,0);				   //7位模式 
	res = ioctl(fd,I2C_SLAVE, (i2c_addr >> 1));    //设置I2C从设备地址
	if(res != 0)
	{
		printf("[%s]:[%d] i2c set ioctl error\r\n", __FUNCTION__, __LINE__);
		close(fd);
		return -1;
	}
	return fd;
}

//读i2c
//buf数据 len长度 
//返回 实际读取的长度
int i2c_readNbyte(int fd, unsigned char *buf, int len)
{
	int res = 0;
	res = read(fd, buf, len);
	return res;
}

//写i2c
//buf数据 len长度 
//返回 实际写的长度 
int i2c_writeNbyte(int fd, unsigned char *buf, int len)
{
	int res = 0;
	res = write(fd, buf, len);
	return res;
}

int i2c_readReg(int fd, unsigned int reg_addr, unsigned char *buf, int len)
{
	int res = 0;
	unsigned char buff[1];
	
	buff[0] = reg_addr;
	write(fd, buff, 1);
	res = read(fd, buf,len);
	
	return res;
}

int i2c_writeReg(int fd, unsigned int reg_addr, unsigned char *buf, int len)
{
	int res = 0,i;
	unsigned char *buff = 0;
	
	buff = (unsigned char *)malloc((len+1));
	buff[0] = reg_addr;
	for(i = 0; i < len; i++)
		buff[(i+1)] = buf[i];
	res = write(fd, buff, (len+1));
	free(buff);
	
	return res;
}

int i2c_close(int fd)
{
	close(fd);
	return 0;
}
