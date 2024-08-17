#include "PWM.h"
#include <math.h>

// Initialize the PWM driver at a specific I2C address
void PWM_init(PWM *pwm, unsigned short i2c_address) {
    pwm->i2cIdentifier = wiringPiI2CSetup(i2c_address);
    if (pwm->i2cIdentifier < 0) {
        printf("Error: Failed to initialize I2C communication\n");
        return;
    }

    // Zero all PWM ports
    PWM_setAllPWM(pwm, 0, 0);

    // Set mode registers
    wiringPiI2CWriteReg8(pwm->i2cIdentifier, PWM_MODE2, PWM_OUTDRV);
    wiringPiI2CWriteReg8(pwm->i2cIdentifier, PWM_MODE1, PWM_ALLCALL | PWM_AI);

    delay(5);  // Wait for oscillator to stabilize

    // Wake up from sleep
    int mode1 = wiringPiI2CReadReg8(pwm->i2cIdentifier, PWM_MODE1);
    mode1 &= ~PWM_SLEEP;
    wiringPiI2CWriteReg8(pwm->i2cIdentifier, PWM_MODE1, mode1);

    delay(5);  // Wait after waking up

    // Set default PWM frequency (e.g., 60 Hz)
    PWM_setPWMFreq(pwm, 60);
}

// Set PWM frequency
void PWM_setPWMFreq(PWM *pwm, float freq) {
    if (pwm->i2cIdentifier < 0) return;

    float prescaleval = 25000000.0;  // 25MHz oscillator
    prescaleval /= 4096.0;           // 12-bit resolution
    prescaleval /= freq;
    prescaleval -= 1.0;

    int prescale = (int)(floor(prescaleval + 0.5));

    int oldmode = wiringPiI2CReadReg8(pwm->i2cIdentifier, PWM_MODE1);
    int sleepmode = (oldmode & 0x7F) | 0x10;  // Set sleep bit
    wiringPiI2CWriteReg8(pwm->i2cIdentifier, PWM_MODE1, sleepmode);

    // Add delay after entering sleep mode
    delay(1);  // Wait 1 millisecond

    wiringPiI2CWriteReg8(pwm->i2cIdentifier, PWM_PRESCALE, prescale);

    // Restore old mode
    wiringPiI2CWriteReg8(pwm->i2cIdentifier, PWM_MODE1, oldmode);

    // Add delay after waking up
    delay(5);  // Wait 5 milliseconds
}

// Set PWM for a specific pin (channel)
int PWM_setPWM(PWM *pwm, unsigned short pin, unsigned short on, unsigned short off) {
    if (pwm->i2cIdentifier < 0) return PWM_ERROR;

    wiringPiI2CWriteReg8(pwm->i2cIdentifier, PWM_LED0_ON_L + 4 * pin, on & 0xFF);
    wiringPiI2CWriteReg8(pwm->i2cIdentifier, PWM_LED0_ON_H + 4 * pin, on >> 8);
    wiringPiI2CWriteReg8(pwm->i2cIdentifier, PWM_LED0_OFF_L + 4 * pin, off & 0xFF);
    wiringPiI2CWriteReg8(pwm->i2cIdentifier, PWM_LED0_OFF_H + 4 * pin, off >> 8);

    return PWM_SUCCESS;
}

// Set PWM values for all channels
int PWM_setAllPWM(PWM *pwm, unsigned short on, unsigned short off) {
    if (pwm->i2cIdentifier < 0) return PWM_ERROR;

    wiringPiI2CWriteReg8(pwm->i2cIdentifier, PWM_ALL_LED_ON_L, on & 0xFF);
    wiringPiI2CWriteReg8(pwm->i2cIdentifier, PWM_ALL_LED_ON_H, on >> 8);
    wiringPiI2CWriteReg8(pwm->i2cIdentifier, PWM_ALL_LED_OFF_L, off & 0xFF);
    wiringPiI2CWriteReg8(pwm->i2cIdentifier, PWM_ALL_LED_OFF_H, off >> 8);

    return PWM_SUCCESS;
}




