#ifndef PWM_H
#define PWM_H

#include <wiringPiI2C.h>
#include <wiringPi.h>
#include <stdio.h>

// Constants
#define PWM_MODE2 0x01
#define PWM_OUTDRV 0x04
#define PWM_MODE1 0x00
#define PWM_ALLCALL 0x01
#define PWM_SLEEP 0x10
#define PWM_PRESCALE 0xFE
#define PWM_ALL_LED_ON_L 0xFA
#define PWM_ALL_LED_ON_H 0xFB
#define PWM_ALL_LED_OFF_L 0xFC
#define PWM_ALL_LED_OFF_H 0xFD
#define PWM_LED0_ON_L 0x06
#define PWM_LED0_ON_H 0x07
#define PWM_LED0_OFF_L 0x08
#define PWM_LED0_OFF_H 0x09
#define PWM_FREQUENCY 1600

typedef struct {
    int i2cIdentifier;
} PWM;

// Function prototypes
void PWM_init(PWM *pwm, unsigned short i2c_address);
int PWM_setAllPWM(PWM *pwm, unsigned short on, unsigned short off);
int PWM_setPWM(PWM *pwm, unsigned short pin, unsigned short on, unsigned short off);
int PWM_setPin(PWM *pwm, unsigned short pin, unsigned short value);
int PWM_getI2cIdentifier(PWM *pwm);

#endif // PWM_H