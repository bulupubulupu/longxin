#include "../include/ls2k500_pwm.h"
#include <stdlib.h>
int main(int argc, char** argv)
{
    struct ls2k_pwm_WQT pwm;
    if(argc != 4)
    {
        printf("Usage : %s <PWM> <frequency> <duty> \n", argv[0]);
        return -1;
    }

    if(memcmp(argv[1], LS2kPWM0)==0)
        pwm.name = LS2kPWM0;
    if(memcmp(argv[1], LS2kPWM1)==0)
        pwm.name = LS2kPWM1;
    if(memcmp(argv[1], LS2kPWM2)==0)
        pwm.name = LS2kPWM2;
    if(memcmp(argv[1], LS2kPWM5)==0)
        pwm.name = LS2kPWM5;
    
    pwm.frequency = atoi(argv[2]);
    pwm.duty = atoi(argv[3]);

    PWM_Init(&pwm);

    while (1)
    {
        gets("/")
    }
    
}
