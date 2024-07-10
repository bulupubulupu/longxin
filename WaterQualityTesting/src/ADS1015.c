/*
 * Copyright (C) 2020-2021 Suzhou Tiancheng Software Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * ads1015.c
 *
 *  Created on: 2015-11-11
 *      Author: Bian
 */

#include "../include/ADS1015.h"
#include "./drivers/ls2k500_i2c.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>

//-----------------------------------------------------------------------------

/*
 * I2C ADDRESS/BITS,
 * 	ADDR:	Ground	1001000
 *			VDD		1001001
 *			SDA		1001010
 *			SCL		1001011
 */
#define ADS1015_ADDRESS         0x48	    /* Ground 1001 000 */

#define ADS1015_BAUDRATE        1000000

//-----------------------------------------------------------------------------

#define CHECK_DONE(rt) \
	do {               \
        if (0 != rt)   \
            goto lbl_done; \
    } while (0);

//-----------------------------------------------------------------------------

static volatile uint16_t saved_config_reg = 0;

/******************************************************************************
 * common functions
 ******************************************************************************/

static void print_ads1015_config_reg(uint16_t val)
{
	printf("ADS1015 Config-Convert Register:\r\n");
	printf("OS = %s\r\n",
			(val & ADS1015_REG_CONFIG_OS_MASK)==ADS1015_REG_CONFIG_OS_NOTBUSY ? "device is not performing a conversion" :
			(val & ADS1015_REG_CONFIG_OS_MASK)==ADS1015_REG_CONFIG_OS_BUSY    ? "conversion is in progress" : "ERROR");
	printf("MUX = %s\r\n",
			(val & ADS1015_REG_CONFIG_MUX_MASK)==ADS1015_REG_CONFIG_MUX_DIFF_0_1 ? "Differential P = AIN0, N = AIN1" :
			(val & ADS1015_REG_CONFIG_MUX_MASK)==ADS1015_REG_CONFIG_MUX_DIFF_0_3 ? "Differential P = AIN0, N = AIN3" :
			(val & ADS1015_REG_CONFIG_MUX_MASK)==ADS1015_REG_CONFIG_MUX_DIFF_1_3 ? "Differential P = AIN1, N = AIN3" :
			(val & ADS1015_REG_CONFIG_MUX_MASK)==ADS1015_REG_CONFIG_MUX_DIFF_2_3 ? "Differential P = AIN2, N = AIN3" :
			(val & ADS1015_REG_CONFIG_MUX_MASK)==ADS1015_REG_CONFIG_MUX_SINGLE_0 ? "Single-ended AIN0" :
			(val & ADS1015_REG_CONFIG_MUX_MASK)==ADS1015_REG_CONFIG_MUX_SINGLE_1 ? "Single-ended AIN1" :
			(val & ADS1015_REG_CONFIG_MUX_MASK)==ADS1015_REG_CONFIG_MUX_SINGLE_2 ? "Single-ended AIN2" :
			(val & ADS1015_REG_CONFIG_MUX_MASK)==ADS1015_REG_CONFIG_MUX_SINGLE_3 ? "Single-ended AIN3" : "ERROR");
	printf("PGA = %s\r\n",
			(val & ADS1015_REG_CONFIG_PGA_MASK)==ADS1015_REG_CONFIG_PGA_6_144V ? "+/-6.144V range, Gain 2/3" :
			(val & ADS1015_REG_CONFIG_PGA_MASK)==ADS1015_REG_CONFIG_PGA_4_096V ? "+/-4.096V range, Gain 1" :
			(val & ADS1015_REG_CONFIG_PGA_MASK)==ADS1015_REG_CONFIG_PGA_2_048V ? "+/-2.048V range, Gain 2" :
			(val & ADS1015_REG_CONFIG_PGA_MASK)==ADS1015_REG_CONFIG_PGA_1_024V ? "+/-1.024V range, Gain 4" :
			(val & ADS1015_REG_CONFIG_PGA_MASK)==ADS1015_REG_CONFIG_PGA_0_512V ? "+/-0.512V range, Gain 8" :
			(val & ADS1015_REG_CONFIG_PGA_MASK)==ADS1015_REG_CONFIG_PGA_0_256V ? "+/-0.256V range, Gain 16" : "ERROR");
	printf("MODE = %s\r\n",
			(val & ADS1015_REG_CONFIG_MODE_MASK)==ADS1015_REG_CONFIG_MODE_CONTIN ? "Continuous conversion mode" :
			(val & ADS1015_REG_CONFIG_MODE_MASK)==ADS1015_REG_CONFIG_MODE_SINGLE ? "Power-down single-shot mode" : "ERROR");
	printf("RATE = %s\r\n",
			(val & ADS1015_REG_CONFIG_DR_MASK)==ADS1015_REG_CONFIG_DR_128SPS  ? "128  samples per second" :
			(val & ADS1015_REG_CONFIG_DR_MASK)==ADS1015_REG_CONFIG_DR_250SPS  ? "250  samples per second" :
			(val & ADS1015_REG_CONFIG_DR_MASK)==ADS1015_REG_CONFIG_DR_490SPS  ? "490  samples per second" :
			(val & ADS1015_REG_CONFIG_DR_MASK)==ADS1015_REG_CONFIG_DR_920SPS  ? "920  samples per second" :
			(val & ADS1015_REG_CONFIG_DR_MASK)==ADS1015_REG_CONFIG_DR_1600SPS ? "1600 samples per second" :
			(val & ADS1015_REG_CONFIG_DR_MASK)==ADS1015_REG_CONFIG_DR_2400SPS ? "2400 samples per second" :
			(val & ADS1015_REG_CONFIG_DR_MASK)==ADS1015_REG_CONFIG_DR_3300SPS ? "3300 samples per second" : "ERROR");
	printf("CMODE = %s\r\n",
			(val & ADS1015_REG_CONFIG_CMODE_MASK)==ADS1015_REG_CONFIG_CMODE_TRAD   ? "Traditional comparator with hysteresis" :
			(val & ADS1015_REG_CONFIG_CMODE_MASK)==ADS1015_REG_CONFIG_CMODE_WINDOW ? "Window comparator" : "ERROR");
	printf("CPOL = %s\r\n",
			(val & ADS1015_REG_CONFIG_CPOL_MASK)==ADS1015_REG_CONFIG_CPOL_ACTVLOW ? "ALERT/RDY pin is low when active" :
			(val & ADS1015_REG_CONFIG_CPOL_MASK)==ADS1015_REG_CONFIG_CPOL_ACTVHI  ? "ALERT/RDY pin is high when active" : "ERROR");
	printf("CPOL = %s\r\n",
			(val & ADS1015_REG_CONFIG_CLAT_MASK)==ADS1015_REG_CONFIG_CLAT_NONLAT ? "Non-latching comparator (default)" :
			(val & ADS1015_REG_CONFIG_CLAT_MASK)==ADS1015_REG_CONFIG_CLAT_LATCH  ? "Latching comparator" : "ERROR");
	printf("CPOL = %s\r\n",
			(val & ADS1015_REG_CONFIG_CQUE_MASK) == ADS1015_REG_CONFIG_CQUE_1CONV ? "Assert ALERT/RDY after one conversions" :
			(val & ADS1015_REG_CONFIG_CQUE_MASK) == ADS1015_REG_CONFIG_CQUE_2CONV ? "Assert ALERT/RDY after two conversions" :
			(val & ADS1015_REG_CONFIG_CQUE_MASK) == ADS1015_REG_CONFIG_CQUE_4CONV ? "Assert ALERT/RDY after four conversions" :
			(val & ADS1015_REG_CONFIG_CQUE_MASK) == ADS1015_REG_CONFIG_CQUE_NONE ? "Disable the comparator and put ALERT/RDY in high state" : "ERROR");
}

/*
 * read or write config/lowthresh/highthresh register
 * rw: FALSE=write TRUE=read
 */
static int ADS1015_reg_access(struct ls2k_adc *pIIC, uint8_t regNum, uint16_t *regVal, bool rw)
{
    int rt = 0, rw_cnt = 0;
	unsigned char buf[4] = { 0 };

	if (regNum >= ADS1015_REGISTER_COUNT)
		return -1;

	if (rw)		/* read */
	{
		rw_cnt = i2c_readReg(pIIC->fd, regNum, buf, 2);
		if (rw_cnt < 0)
		    rt = rw_cnt;
		CHECK_DONE(rt);

		*regVal = ((uint16_t)buf[0] << 8) + buf[1];
	}
	else		/* write */
	{
		
		buf[0] = *regVal >> 8;
		buf[1] = *regVal & 0xff;
		i2c_writeReg(pIIC->fd, regNum, buf, 2);
	}

lbl_done:

	return rt;
}

/******************************************************************************
 * driver implement
 ******************************************************************************/

/*
 * read convert result of ads1015, ONLY CONTINUOUS MODE
 */
static int ADS1015_read(void *bus, void *buf, int size, void *arg)
{
    int rt = 0, channel = (int)arg;
	uint16_t *pVal = (uint16_t *)buf, cfgVal;

    if ((bus == NULL) || (buf == NULL))
        return -1;

    if (saved_config_reg == 0)    /* The first time read */
    {
        saved_config_reg = ADS1015_CFG_DEFAULT; // use deault config
    }

    if (channel > 0)            /* Parameter has channel to read */
    {
        cfgVal = saved_config_reg & ~ADS1015_REG_CONFIG_MUX_MASK;

        switch (channel)
        {
            case ADS1015_CHANNEL_D0: cfgVal |= ADS1015_REG_CONFIG_MUX_DIFF_0_1; break;
            case ADS1015_CHANNEL_D1: cfgVal |= ADS1015_REG_CONFIG_MUX_DIFF_0_3; break;
            case ADS1015_CHANNEL_D2: cfgVal |= ADS1015_REG_CONFIG_MUX_DIFF_1_3; break;
            case ADS1015_CHANNEL_D3: cfgVal |= ADS1015_REG_CONFIG_MUX_DIFF_2_3; break;

            case ADS1015_CHANNEL_S0: cfgVal |= ADS1015_REG_CONFIG_MUX_SINGLE_0; break;
            case ADS1015_CHANNEL_S1: cfgVal |= ADS1015_REG_CONFIG_MUX_SINGLE_1; break;
            case ADS1015_CHANNEL_S2: cfgVal |= ADS1015_REG_CONFIG_MUX_SINGLE_2; break;
            case ADS1015_CHANNEL_S3: cfgVal |= ADS1015_REG_CONFIG_MUX_SINGLE_3; break;
        
            default: cfgVal = 0; break;
        }
        
        if (cfgVal != 0)        /* Channel is ok */
        {
            rt = ADS1015_reg_access((struct ls2k_adc *)bus,
                                    ADS1015_REG_POINTER_CONFIG,
                                    &cfgVal,
                                    false); // write
            if (rt != 2)
                return rt;
                
            sleep(2);
        }
    }

	/******************************************************
	 * read convert register
	 ******************************************************/

	rt = ADS1015_reg_access((struct ls2k_adc *)bus,
                            ADS1015_REG_POINTER_CONVERT,
                            pVal,
                            true); // read
	if (rt == 0)
		*pVal >>= 4;

	return rt;
}

 int ADS1015_ioctl(void *bus, int cmd, void *arg)
{
	int rt = 0;
	uint16_t val = 0;
    struct ls2k_adc *pIIC = (struct ls2k_adc *)bus;

    if(bus == NULL)
	{
    	return -1;
	}
        
	switch (cmd)
	{
		/* access full-config register */
		case IOCTL_ADS1015_SET_CONV_CTRL:
			val = (uint16_t)((unsigned)arg);
			rt = ADS1015_reg_access(pIIC, ADS1015_REG_POINTER_CONFIG, &val, false);
			saved_config_reg = val;
			break;

		case IOCTL_ADS1015_GET_CONV_CTRL:
			rt = ADS1015_reg_access(pIIC, ADS1015_REG_POINTER_CONFIG, &val, true);
			*(uint16_t *)arg = val;
            saved_config_reg = val;
			break;

		/* access low thresh register */
		case IOCTL_ADS1015_SET_LOW_THRESH:
			val = (uint16_t)((unsigned)arg);
			rt = ADS1015_reg_access(pIIC, ADS1015_REG_POINTER_LOWTHRESH, &val, false);
			break;

		case IOCTL_ADS1015_GET_LOW_THRESH:
			rt = ADS1015_reg_access(pIIC, ADS1015_REG_POINTER_LOWTHRESH, &val, true);
			*(uint16_t *)arg = val;
			break;

		/* access high thresh register */
		case IOCTL_ADS1015_SET_HIGH_THRESH:
			val = (uint16_t)((unsigned)arg);
			rt = ADS1015_reg_access(pIIC, ADS1015_REG_POINTER_HITHRESH, &val, false);
			break;

		case IOCTL_ADS1015_GET_HIGH_THRESH:
			rt = ADS1015_reg_access(pIIC, ADS1015_REG_POINTER_HITHRESH, &val, true);
			*(uint16_t *)arg = val;
			break;

		/* display config register */
		case IOCTL_ADS1015_DISP_CONFIG_REG:
		{
			rt = ADS1015_reg_access(pIIC, ADS1015_REG_POINTER_CONFIG, &val, true);
			if (rt == 0)
				print_ads1015_config_reg(val);
			break;
		}

		default:
			break;
	}

	return rt;
}

/******************************************************************************
 * user api
 */
uint16_t get_ads1015_adc(void *bus, int channel)
{
    if (ADS1015_ioctl(bus,
                      IOCTL_ADS1015_SET_CONV_CTRL,
                      (void *)(channel | ADS1015_CFG_DEFAULT)) >= 0)
    {
        uint16_t val;
        sleep(2);
        if (ADS1015_read(bus, (void*)&val, 2, NULL) >= 0)
            return val;
    }

    return 0;
}

int ADS1015_Init(struct ls2k_adc* adc ,char* path)
{
	int res = 0;
	res = i2c_init(path, ADS1015_ADDRESS);
	if(res == -1)
	{
		return -1;
	}
	adc->fd = res;
	return 0;
}
