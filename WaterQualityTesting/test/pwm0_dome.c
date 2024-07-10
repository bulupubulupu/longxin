#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PWM_CHIP_PATH "/sys/class/pwm/pwmchip0/"
#define PWM_EXPORT_PATH PWM_CHIP_PATH "export"
#define PWM_UNEXPORT_PATH PWM_CHIP_PATH "unexport"
#define PWM_PERIOD_PATH PWM_CHIP_PATH "pwm0/period"
#define PWM_DUTY_CYCLE_PATH PWM_CHIP_PATH "pwm0/duty_cycle"
#define PWM_ENABLE_PATH PWM_CHIP_PATH "pwm0/enable"

#define SECONDS_IN_A_CYCLE 1000000000

// 函数声明
int export_pwm();
int unexport_pwm();
int set_pwm_period(unsigned int period_ns);
int set_pwm_duty_cycle(unsigned int duty_ns, unsigned char level_control);
int enable_pwm();
int disable_pwm();
unsigned int frequency_to_period(unsigned int frequency);
unsigned int percent_to_us(unsigned int percent, unsigned int period);

int main(int argc, char **argv) {

    unsigned int frequency_hz = 50;     //频率（HZ） 
    unsigned int period_ns = 0;  // pwm周期（纳秒）
    unsigned int duty_percent = 50;     // 占空比（%）
    unsigned int duty_ns = 0;     
    unsigned char level_control = 0;    //电平极性
    
	if(argc < 3)
    {
        printf("Usage: %s <frequency> <duty> [lowLevel]\n", argv[0]);
		return -1;
    }

    if(argc == 4 && !strcmp(argv[2], "lowLevel"))
    {
        level_control = 1;
    }

    frequency_hz = atoi(argv[1]);
    duty_ns = atoi(argv[2]);

    // 导出 PWM
    if (export_pwm() != 0) {
        printf("Error: Unable to export PWM\n");
        return -1;
    }

    period_ns = frequency_to_period(frequency_hz);
    printf("period_ns = %d\n", period_ns);
    if(period_ns == 0)
    {
        printf("Error: Unable to set PWM frequency\n");
        unexport_pwm();
        return -1;
    }

    // 设置 PWM 周期（单位：纳秒）
    if (set_pwm_period(period_ns) != 0) {
        printf("Error: Unable to set PWM period\n");
        unexport_pwm();
        return -1;
    }

    // 设置 PWM 占空比（单位：纳秒）
    // duty_ns = percent_to_us(duty_percent, period_ns);
    if (set_pwm_duty_cycle(duty_ns, level_control) != 0) {
        printf("Error: Unable to set PWM duty cycle\n");
        unexport_pwm();
        return -1;
    }

    // 启用 PWM
    if (enable_pwm() != 0) {
        printf("Error: Unable to enable PWM\n");
        unexport_pwm();
        return -1;
    }

    // 等待一段时间，然后禁用 PWM
    usleep(10000000); // 等待 5 秒
    disable_pwm();

    // 取消导出 PWM
    unexport_pwm();

    return 0;
}

// 导出 PWM
int export_pwm() {
    FILE *export_file = fopen(PWM_EXPORT_PATH, "w");
    if (export_file == NULL) {
        return 1;
    }
    fprintf(export_file, "0"); // 导出 PWM0
    fclose(export_file);
    return 0;
}

// 取消导出 PWM
int unexport_pwm() {
    FILE *unexport_file = fopen(PWM_UNEXPORT_PATH, "w");
    if (unexport_file == NULL) {
        return 1;
    }
    fprintf(unexport_file, "0"); // 取消导出 PWM0
    fclose(unexport_file);
    return 0;
}

// 设置 PWM 周期
int set_pwm_period(unsigned int period_ns) {
    FILE *period_file = fopen(PWM_PERIOD_PATH, "w");
    if (period_file == NULL) {
        return 1;
    }
    fprintf(period_file, "%u", period_ns);
    fclose(period_file);
    return 0;
}

// 设置 PWM 占空比
int set_pwm_duty_cycle(unsigned int duty_ns, unsigned char level_control) {
    FILE *duty_cycle_file = fopen(PWM_DUTY_CYCLE_PATH, "w");
    if (duty_cycle_file == NULL) {
        return 1;
    }
    fprintf(duty_cycle_file, "%u", duty_ns);
    fclose(duty_cycle_file);
    return 0;
}

// 启用 PWM
int enable_pwm() {
    FILE *enable_file = fopen(PWM_ENABLE_PATH, "w");
    if (enable_file == NULL) {
        return 1;
    }
    fprintf(enable_file, "1");
    fclose(enable_file);
    return 0;
}

// 禁用 PWM
int disable_pwm() {
    FILE *enable_file = fopen(PWM_ENABLE_PATH, "w");
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
    unsigned int period = 1000000000 / frequency;
    return period;
}

unsigned int percent_to_us(unsigned int percent, unsigned int period)
{
    double duty_us = period * percent / 100;

    return (unsigned int)duty_us;
}
