#include "PWM.h"

void PWM_init(PWM *pwm, unsigned short i2c_address) {
    pwm->i2cIdentifier = wiringPiI2CSetup(i2c_address);

    // Setup PWM
    PWM_setAllPWM(pwm, 0, 0);
    wiringPiI2CWriteReg8(pwm->i2cIdentifier, PWM_MODE2, PWM_OUTDRV);
    wiringPiI2CWriteReg8(pwm->i2cIdentifier, PWM_MODE1, PWM_ALLCALL);
    delay(5);
    unsigned short mode1 = wiringPiI2CReadReg8(pwm->i2cIdentifier, PWM_MODE1) & ~PWM_SLEEP;
    wiringPiI2CWriteReg8(pwm->i2cIdentifier, PWM_MODE1, mode1);
    delay(5);

    // Set PWM frequency
    unsigned short prescale = (int)(25000000.0 / 4096.0 / PWM_FREQUENCY - 1.0);
    unsigned short oldmode = wiringPiI2CReadReg8(pwm->i2cIdentifier, PWM_MODE1);
    unsigned short newmode = (oldmode & 0x7F) | 0x10;
    wiringPiI2CWriteReg8(pwm->i2cIdentifier, PWM_MODE1, newmode);
    wiringPiI2CWriteReg8(pwm->i2cIdentifier, PWM_PRESCALE, prescale);
    wiringPiI2CWriteReg8(pwm->i2cIdentifier, PWM_MODE1, oldmode);
    delay(5);
    wiringPiI2CWriteReg8(pwm->i2cIdentifier, PWM_MODE1, oldmode | 0x80);
}

int PWM_setAllPWM(PWM *pwm, unsigned short on, unsigned short off) {
    if (wiringPiI2CWriteReg8(pwm->i2cIdentifier, PWM_ALL_LED_ON_L, on & 0xFF) ||
        wiringPiI2CWriteReg8(pwm->i2cIdentifier, PWM_ALL_LED_ON_H, on >> 8) ||
        wiringPiI2CWriteReg8(pwm->i2cIdentifier, PWM_ALL_LED_OFF_L, off & 0xFF) ||
        wiringPiI2CWriteReg8(pwm->i2cIdentifier, PWM_ALL_LED_OFF_H, off >> 8)) {
        return -1;
    }
    return 0;
}

int PWM_setPWM(PWM *pwm, unsigned short pin, unsigned short on, unsigned short off) {
    if (wiringPiI2CWriteReg8(pwm->i2cIdentifier, PWM_LED0_ON_L + 4 * pin, on & 0xFF) ||
        wiringPiI2CWriteReg8(pwm->i2cIdentifier, PWM_LED0_ON_H + 4 * pin, on >> 8) ||
        wiringPiI2CWriteReg8(pwm->i2cIdentifier, PWM_LED0_OFF_L + 4 * pin, off & 0xFF) ||
        wiringPiI2CWriteReg8(pwm->i2cIdentifier, PWM_LED0_OFF_H + 4 * pin, off >> 8)) {
        return -1;
    }
    return 0;
}

int PWM_setPin(PWM *pwm, unsigned short pin, unsigned short value) {
    if (pin > 15) {
        printf("PWM pin must be between 0 and 15 inclusive. Received '%d'\n", pin);
        return -1;
    }
    switch (value) {
        case 0:
            return PWM_setPWM(pwm, pin, 0, 4096);
        case 1:
            return PWM_setPWM(pwm, pin, 4096, 0);
        default:
            printf("PWM pin value must be 0 or 1. Received '%d'\n", value);
            return -1;
    }
}

int PWM_getI2cIdentifier(PWM *pwm) {
    return pwm->i2cIdentifier;
}