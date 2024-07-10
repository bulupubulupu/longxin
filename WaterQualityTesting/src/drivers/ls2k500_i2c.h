#ifndef __LS2k500_I2C_H_
#define __LS2k500_I2C_H_

int i2c_init(char *i2c_dev, unsigned char i2c_addr);
int i2c_readNbyte(int fd, unsigned char *buf, int len);
int i2c_writeNbyte(int fd, unsigned char *buf, int len);
int i2c_readReg(int fd, unsigned int reg_addr, unsigned char *buf, int len);
int i2c_writeReg(int fd, unsigned int reg_addr, unsigned char *buf, int len);
int i2c_close(int fd);

#endif
