#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ls2k500_pwm.h"

int PWM_Init(struct ls2k_pwm_WQT *tpwm)
{
    if(tpwm == NULL)
    {
        printf("ls2k_pwm_WQT init error!\r\n");
        return -1;
    }
    if(memcmp(tpwm->name,LS2kPWM0,4)==0 || memcmp(tpwm->name,LS2kPWM1,4)==0 || memcmp(tpwm->name,LS2kPWM2,4)==0 || memcmp(tpwm->name,LS2kPWM5,4)==0)
    {
        if(memcmp(tpwm->name,LS2kPWM0,4)==0)
        {
            tpwm->chip_path = LS2k_PWM_CHIP_PAYH0;
            tpwm->duty_cycle_path = PWM0_DUTY_CYCLE_PATH;
            tpwm->enable_path = PWM0_ENABLE_PATH;
            tpwm->export_path = PWM0_EXPORT_PATH;
            tpwm->period_path = PWM0_PERIOD_PATH;
            tpwm->unexport_path = PWM0_UNEXPORT_PATH;   
        }
        if(memcmp(tpwm->name,LS2kPWM1,4)==0)
        {
            tpwm->chip_path = LS2k_PWM_CHIP_PAYH1;
            tpwm->duty_cycle_path = PWM1_DUTY_CYCLE_PATH;
            tpwm->enable_path = PWM1_ENABLE_PATH;
            tpwm->export_path = PWM1_EXPORT_PATH;
            tpwm->period_path = PWM1_PERIOD_PATH;
            tpwm->unexport_path = PWM1_UNEXPORT_PATH;   
        }
        if(memcmp(tpwm->name,LS2kPWM2,4)==0)
        {
            tpwm->chip_path = LS2k_PWM_CHIP_PAYH2;
            tpwm->duty_cycle_path = PWM2_DUTY_CYCLE_PATH;
            tpwm->enable_path = PWM2_ENABLE_PATH;
            tpwm->export_path = PWM2_EXPORT_PATH;
            tpwm->period_path = PWM2_PERIOD_PATH;
            tpwm->unexport_path = PWM2_UNEXPORT_PATH;   
        }
        if(memcmp(tpwm->name,LS2kPWM5,4)==0)
        {
            tpwm->chip_path = LS2k_PWM_CHIP_PAYH5;
            tpwm->duty_cycle_path = PWM5_DUTY_CYCLE_PATH;
            tpwm->enable_path = PWM5_ENABLE_PATH;
            tpwm->export_path = PWM5_EXPORT_PATH;
            tpwm->period_path = PWM5_PERIOD_PATH;
            tpwm->unexport_path = PWM5_UNEXPORT_PATH;   
        }
        
        tpwm->period = frequency_to_period(tpwm->frequency);
        if(export_pwm(tpwm->export_path) != 0)
        {
            printf("Error: Unable to export %s\n", tpwm->name);
            return -1;
        }
        if (set_pwm_period(tpwm->period_path, tpwm->period) != 0) {
            printf("Error: Unable to set %s period\n", tpwm->name);
            unexport_pwm(tpwm->unexport_path);
            return -1;
        }
        if (set_pwm_duty_cycle(tpwm->duty_cycle_path, tpwm->duty, tpwm->polarity) != 0) {
            printf("Error: Unable to set %s duty cycle\n", tpwm->name);
            unexport_pwm(tpwm->unexport_path);
            return -1;
        }
        return 0;
    }
    if(memcmp(tpwm->name,LS2kPWM3,4)==0 || memcmp(tpwm->name,LS2kPWM4,4)==0)
    {
        printf("ls2k500 don't have pwm3 or pwm4!\n");
        return -1;
    }

    printf("pwm init error! Please check that the Settings are correct.\n");
    return -1;
}

int PWM_Start(struct ls2k_pwm_WQT *tpwm)
{
    if (enable_pwm(tpwm->enable_path) != 0) {
        printf("Error: Unable to enable %s\n", tpwm->name);
        return -1;
    }
    return 0;
}

int PWM_Down(struct ls2k_pwm_WQT *tpwm)
{
    if(disable_pwm(tpwm->enable_path) != 0)
    {
        printf("Error: Unable to enable %s\n", tpwm->name);
        return -1;
    }
    return 0;
}

int PWM_Delete(struct ls2k_pwm_WQT *tpwm)
{
    unexport_pwm(tpwm->unexport_path);
    return 0;
}

int PWM_SetDuty(struct ls2k_pwm_WQT *tpwm, unsigned int duty)
{
    tpwm->duty=duty;
    if (set_pwm_duty_cycle(tpwm->duty_cycle_path, tpwm->duty, tpwm->polarity) != 0) {
        printf("Error: Unable to set %s duty cycle\n", tpwm->name);
        return -1;
    }
    return 0;
}

// 导出 PWM
int export_pwm(const char *path) {
    FILE *export_file = fopen(path, "w");
    if (export_file == NULL) {
        return 1;
    }
    fprintf(export_file, "0"); // 导出 PWM0
    fclose(export_file);
    return 0;
}

// 取消导出 PWM
int unexport_pwm(const char* path) {
    FILE *unexport_file = fopen(path, "w");
    if (unexport_file == NULL) {
        return 1;
    }
    fprintf(unexport_file, "0"); // 取消导出 PWM0
    fclose(unexport_file);
    return 0;
}

// 设置 PWM 周期
int set_pwm_period(const char* path, unsigned int period_ns) {
    FILE *period_file = fopen(path, "w");
    if (period_file == NULL) {
        return 1;
    }
    fprintf(period_file, "%u", period_ns);
    fclose(period_file);
    return 0;
}

// 设置 PWM 占空比
int set_pwm_duty_cycle(const char* path, unsigned int duty, unsigned char polarity) {
    FILE *duty_cycle_file = fopen(path, "w");
    if (duty_cycle_file == NULL) {
        return 1;
    }
    fprintf(duty_cycle_file, "%u", duty);
    fclose(duty_cycle_file);
    return 0;
}

// 启用 PWM
int enable_pwm(const char* path) {
    FILE *enable_file = fopen(path, "w");
    if (enable_file == NULL) {
        return 1;
    }
    fprintf(enable_file, "1");
    fclose(enable_file);
    return 0;
}

// 禁用 PWM
int disable_pwm(const char* path) {
    FILE *enable_file = fopen(path, "w");
    if (enable_file == NULL) {
        return 1;
    }
    fprintf(enable_file, "0");
    fclose(enable_file);
    return 0;
}

unsigned int frequency_to_period(unsigned int frequency) {
    // 计算周期
    if(frequency == 0)
    {
        return 0;
    }
    unsigned int period = SECONDS_IN_A_CYCLE / frequency;
    return period;
}

unsigned int percent_to_us(unsigned int percent, unsigned int period)
{
    double duty_us = period * percent / 100;

    return (unsigned int)duty_us;
}
