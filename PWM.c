#include "PWM.h"
#include <math.h>  // Needed for frequency calculation

// Define success and error constants
#define PWM_SUCCESS 0
#define PWM_ERROR -1

// Initialize the PWM driver
void PWM_init(PWM *pwm, unsigned short i2c_address) {
    pwm->i2cIdentifier = wiringPiI2CSetup(i2c_address);
    
    if (pwm->i2cIdentifier == -1) {
        printf("Error: Unable to initialize I2C at address 0x%02X\n", i2c_address);
        return;
    }
    
    // Set the mode registers to default
    wiringPiI2CWriteReg8(pwm->i2cIdentifier, PWM_MODE2, PWM_OUTDRV);
    wiringPiI2CWriteReg8(pwm->i2cIdentifier, PWM_MODE1, PWM_ALLCALL);
    
    // Wait for oscillator to stabilize
    delay(5);
    
    // Wake up from sleep mode
    int mode1 = wiringPiI2CReadReg8(pwm->i2cIdentifier, PWM_MODE1);
    mode1 &= ~PWM_SLEEP;  // Clear sleep bit to wake up
    wiringPiI2CWriteReg8(pwm->i2cIdentifier, PWM_MODE1, mode1);
    
    // Wait for oscillator again
    delay(5);
}

// Set PWM frequency
void PWM_setPWMFreq(PWM *pwm, float freq) {
    float prescaleval = 25000000.0;  // 25 MHz
    prescaleval /= 4096.0;           // 12-bit resolution
    prescaleval /= freq;
    prescaleval -= 1.0;
    unsigned char prescale = (unsigned char)(floor(prescaleval + 0.5));
    
    int oldmode = wiringPiI2CReadReg8(pwm->i2cIdentifier, PWM_MODE1);
    int newmode = (oldmode & 0x7F) | 0x10;  // Sleep mode
    wiringPiI2CWriteReg8(pwm->i2cIdentifier, PWM_MODE1, newmode);  // Go to sleep
    wiringPiI2CWriteReg8(pwm->i2cIdentifier, PWM_PRESCALE, prescale);
    wiringPiI2CWriteReg8(pwm->i2cIdentifier, PWM_MODE1, oldmode);
    delay(5);
    wiringPiI2CWriteReg8(pwm->i2cIdentifier, PWM_MODE1, oldmode | 0x80);  // Restart
}

// Set PWM for all channels
int PWM_setAllPWM(PWM *pwm, unsigned short on, unsigned short off) {
    if (wiringPiI2CWriteReg8(pwm->i2cIdentifier, PWM_ALL_LED_ON_L, on & 0xFF) == -1 ||
        wiringPiI2CWriteReg8(pwm->i2cIdentifier, PWM_ALL_LED_ON_H, on >> 8) == -1 ||
        wiringPiI2CWriteReg8(pwm->i2cIdentifier, PWM_ALL_LED_OFF_L, off & 0xFF) == -1 ||
        wiringPiI2CWriteReg8(pwm->i2cIdentifier, PWM_ALL_LED_OFF_H, off >> 8) == -1) {
        printf("Error writing to all PWM channels\n");
        return PWM_ERROR;
    }
    
    return PWM_SUCCESS;  // Return success
}

// Set PWM for a specific pin
int PWM_setPWM(PWM *pwm, unsigned short pin, unsigned short on, unsigned short off) {
    if (pin >= 16) {
        printf("Error: PWM pin out of range (0-15)\n");
        return PWM_ERROR;
    }
    
    int base_address = PWM_LED0_ON_L + 4 * pin;
    if (wiringPiI2CWriteReg8(pwm->i2cIdentifier, base_address, on & 0xFF) == -1 ||
        wiringPiI2CWriteReg8(pwm->i2cIdentifier, base_address + 1, on >> 8) == -1 ||
        wiringPiI2CWriteReg8(pwm->i2cIdentifier, base_address + 2, off & 0xFF) == -1 ||
        wiringPiI2CWriteReg8(pwm->i2cIdentifier, base_address + 3, off >> 8) == -1) {
        printf("Error writing to PWM pin %d\n", pin);
        return PWM_ERROR;
    }
    
    return PWM_SUCCESS;  // Return success
}

// Set a pin to a specific value (0 for off, 4096 for fully on)
int PWM_setPin(PWM *pwm, unsigned short pin, unsigned short value) {
    if (pin >= 16) {
        printf("Error: PWM pin out of range (0-15)\n");
        return PWM_ERROR;
    }
    
    if (value >= 4096) {
        // Fully on
        return PWM_setPWM(pwm, pin, 4096, 0);
    } else if (value == 0) {
        // Fully off
        return PWM_setPWM(pwm, pin, 0, 4096);
    } else {
        // Set PWM
        return PWM_setPWM(pwm, pin, 0, value);
    }
}

// Get the I2C identifier
int PWM_getI2cIdentifier(PWM *pwm) {
    return pwm->i2cIdentifier;
}

