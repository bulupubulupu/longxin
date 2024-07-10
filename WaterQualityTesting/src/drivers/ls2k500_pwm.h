#ifndef _LS2K500_PWM_H_
#define _LS2K500_PWM_H_

#include <stdio.h>

#define LS2kPWM0 "pwm0"
#define LS2kPWM1 "pwm1"
#define LS2kPWM2 "pwm2"
#define LS2kPWM3 "pwm3"
#define LS2kPWM4 "pwm4"
#define LS2kPWM5 "pwm5"

#define LS2k_CHIP_PWM0 "pwmchip0/"
#define LS2k_CHIP_PWM1 "pwmchip1/"
#define LS2k_CHIP_PWM2 "pwmchip2/"
#define LS2k_CHIP_PWM3 "pwmchip3/"
#define LS2k_CHIP_PWM4 "pwmchip4/"
#define LS2k_CHIP_PWM5 "pwmchip5/"

#define PWM_CHIP_PAYH "/sys/class/pwm/"

#define LS2k_PWM_CHIP_PAYH0 PWM_CHIP_PAYH LS2k_CHIP_PWM0
#define LS2k_PWM_CHIP_PAYH1 PWM_CHIP_PAYH LS2k_CHIP_PWM1
#define LS2k_PWM_CHIP_PAYH2 PWM_CHIP_PAYH LS2k_CHIP_PWM2
#define LS2k_PWM_CHIP_PAYH3 PWM_CHIP_PAYH LS2k_CHIP_PWM3
#define LS2k_PWM_CHIP_PAYH4 PWM_CHIP_PAYH LS2k_CHIP_PWM4
#define LS2k_PWM_CHIP_PAYH5 PWM_CHIP_PAYH LS2k_CHIP_PWM5

#define PWM0_EXPORT_PATH LS2k_PWM_CHIP_PAYH0 "export"
#define PWM1_EXPORT_PATH LS2k_PWM_CHIP_PAYH1 "export"
#define PWM2_EXPORT_PATH LS2k_PWM_CHIP_PAYH2 "export"
#define PWM3_EXPORT_PATH LS2k_PWM_CHIP_PAYH3 "export"
#define PWM4_EXPORT_PATH LS2k_PWM_CHIP_PAYH4 "export"
#define PWM5_EXPORT_PATH LS2k_PWM_CHIP_PAYH5 "export"

#define PWM0_UNEXPORT_PATH LS2k_PWM_CHIP_PAYH0 "unexport" 
#define PWM1_UNEXPORT_PATH LS2k_PWM_CHIP_PAYH1 "unexport" 
#define PWM2_UNEXPORT_PATH LS2k_PWM_CHIP_PAYH2 "unexport" 
#define PWM3_UNEXPORT_PATH LS2k_PWM_CHIP_PAYH3 "unexport" 
#define PWM4_UNEXPORT_PATH LS2k_PWM_CHIP_PAYH4 "unexport" 
#define PWM5_UNEXPORT_PATH LS2k_PWM_CHIP_PAYH5 "unexport" 

#define PWM0_PERIOD_PATH LS2k_PWM_CHIP_PAYH0 LS2kPWM0 "/period"
#define PWM1_PERIOD_PATH LS2k_PWM_CHIP_PAYH1 LS2kPWM1 "/period"
#define PWM2_PERIOD_PATH LS2k_PWM_CHIP_PAYH2 LS2kPWM2 "/period"
#define PWM3_PERIOD_PATH LS2k_PWM_CHIP_PAYH3 LS2kPWM3 "/period"
#define PWM4_PERIOD_PATH LS2k_PWM_CHIP_PAYH4 LS2kPWM4 "/period"
#define PWM5_PERIOD_PATH LS2k_PWM_CHIP_PAYH5 LS2kPWM5 "/period"

#define PWM0_DUTY_CYCLE_PATH LS2k_PWM_CHIP_PAYH0 LS2kPWM0 "/duty_cycle"
#define PWM1_DUTY_CYCLE_PATH LS2k_PWM_CHIP_PAYH1 LS2kPWM1 "/duty_cycle"
#define PWM2_DUTY_CYCLE_PATH LS2k_PWM_CHIP_PAYH2 LS2kPWM2 "/duty_cycle"
#define PWM3_DUTY_CYCLE_PATH LS2k_PWM_CHIP_PAYH3 LS2kPWM3 "/duty_cycle"
#define PWM4_DUTY_CYCLE_PATH LS2k_PWM_CHIP_PAYH4 LS2kPWM4 "/duty_cycle"
#define PWM5_DUTY_CYCLE_PATH LS2k_PWM_CHIP_PAYH5 LS2kPWM5 "/duty_cycle"

#define PWM0_ENABLE_PATH LS2k_PWM_CHIP_PAYH0 LS2kPWM0 "/enable"
#define PWM1_ENABLE_PATH LS2k_PWM_CHIP_PAYH1 LS2kPWM1 "/enable"
#define PWM2_ENABLE_PATH LS2k_PWM_CHIP_PAYH2 LS2kPWM2 "/enable"
#define PWM3_ENABLE_PATH LS2k_PWM_CHIP_PAYH3 LS2kPWM3 "/enable"
#define PWM4_ENABLE_PATH LS2k_PWM_CHIP_PAYH4 LS2kPWM4 "/enable"
#define PWM5_ENABLE_PATH LS2k_PWM_CHIP_PAYH5 LS2kPWM5 "/enable"

#define SECONDS_IN_A_CYCLE 1000000000

struct ls2k_pwm_WQT
{
    char *name;             //名称
    unsigned int frequency; //频率
    unsigned int period;    //周期
    unsigned char polarity; //电平极性
    unsigned int duty;      //占空比

    const char* chip_path;
    const char* export_path;
    const char* unexport_path;
    const char* period_path;
    const char* duty_cycle_path;
    const char* enable_path;
    /* data */
};

// 函数声明
int PWM_Init(struct ls2k_pwm_WQT *tpwm);
int PWM_Start(struct ls2k_pwm_WQT *tpwm);
int PWM_Down(struct ls2k_pwm_WQT *tpwm);
int PWM_Delete(struct ls2k_pwm_WQT *tpwm);
int PWM_SetDuty(struct ls2k_pwm_WQT *tpwm, unsigned int duty);

int export_pwm(const char *path);
int unexport_pwm(const char *path);
int set_pwm_period(const char *path, unsigned int period_ns);
int set_pwm_duty_cycle(const char *path, unsigned int duty_ns, unsigned char level_control);
int enable_pwm(const char *path);
int disable_pwm(const char *path);
unsigned int frequency_to_period(unsigned int frequency);
unsigned int percent_to_us(unsigned int percent, unsigned int period);

#endif
