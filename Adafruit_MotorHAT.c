#include "Adafruit_MotorHAT.h"
#include "PWM.h"
#include <unistd.h>  // For usleep function

// Initialize the Motor HAT
void Adafruit_MotorHAT_init(Adafruit_MotorHAT *hat, int addr, int freq, int i2c, int i2c_bus) {
    (void)i2c;      // Mark i2c as unused
    (void)i2c_bus;  // Mark i2c_bus as unused
    hat->_frequency = freq;
    PWM_init(&hat->pwm, addr);  // Initialize the PWM with the given I2C address

    // Set the frequency for PWM
    PWM_setPWMFreq(&hat->pwm, freq);

    // Initialize stepper motors
    for (int i = 0; i < 2; i++) {
        Adafruit_StepperMotor_init(&hat->steppers[i], hat, i + 1, 200);  // Assuming 200 steps/rev
    }

    // Initialize DC motors
    for (int i = 0; i < 4; i++) {
        Adafruit_DCMotor_init(&hat->dcs[i], hat, i + 1);
    }
}

// Set a pin value (0 or 1)
void Adafruit_MotorHAT_setPin(Adafruit_MotorHAT *hat, int pin, int value) {
    (void)hat;  // Mark hat as unused
    digitalWrite(pin, value);
}

// Get a stepper motor by index (1 or 2)
Adafruit_StepperMotor* Adafruit_MotorHAT_getStepper(Adafruit_MotorHAT *hat, int num) {
    if (num < 1 || num > 2) {
        return NULL;
    }
    return &hat->steppers[num - 1];
}

// Get a DC motor by index (1 to 4)
Adafruit_DCMotor* Adafruit_MotorHAT_getDC(Adafruit_MotorHAT *hat, int num) {
    if (num < 1 || num > 4) {
        return NULL;
    }
    return &hat->dcs[num - 1];
}

// Reset all motors
void Adafruit_MotorHAT_resetAll(Adafruit_MotorHAT *hat) {
    PWM_setAllPWM(&hat->pwm, 0, 0);  // Use the stored PWM instance to turn off all motors
}

// Initialize a stepper motor
void Adafruit_StepperMotor_init(Adafruit_StepperMotor *motor, Adafruit_MotorHAT *hat, int num, int steps) {
    motor->hat = hat;
    motor->revsteps = steps;
    motor->motornum = num;
    motor->currentstep = 0;

    // Assign PWM pins and control pins based on motor number
    if (num == 1) {
        motor->PWMA = 8;
        motor->AIN2 = 9;
        motor->AIN1 = 10;
        motor->PWMB = 13;
        motor->BIN2 = 12;
        motor->BIN1 = 11;
    } else if (num == 2) {
        motor->PWMA = 2;
        motor->AIN2 = 3;
        motor->AIN1 = 4;
        motor->PWMB = 7;
        motor->BIN2 = 6;
        motor->BIN1 = 5;
    }
}

// Set the speed of the stepper motor in RPM
void Adafruit_StepperMotor_setSpeed(Adafruit_StepperMotor *motor, int rpm) {
    motor->sec_per_step = 60.0 / (motor->revsteps * rpm);  // Calculate seconds per step
}

// Step the motor one step
int Adafruit_StepperMotor_oneStep(Adafruit_StepperMotor *motor, Direction dir, Style style) {
    printf("Adafruit_StepperMotor_oneStep: direction=%d, style=%d, current step=%d\n", dir, style, motor->currentstep);

    // The actual stepping logic based on the direction and style
    int step_number = motor->currentstep % 4;  // 4-step sequence
    if (dir == FORWARD) {
        motor->currentstep++;
    } else {
        motor->currentstep--;
    }

    // Implement coil activation for SINGLE style
    if (style == SINGLE) {
        switch (step_number) {
            case 0:
                Adafruit_MotorHAT_setPin(motor->hat, motor->AIN1, HIGH);
                Adafruit_MotorHAT_setPin(motor->hat, motor->AIN2, LOW);
                Adafruit_MotorHAT_setPin(motor->hat, motor->BIN1, LOW);
                Adafruit_MotorHAT_setPin(motor->hat, motor->BIN2, HIGH);
                break;
            case 1:
                Adafruit_MotorHAT_setPin(motor->hat, motor->AIN1, LOW);
                Adafruit_MotorHAT_setPin(motor->hat, motor->AIN2, HIGH);
                Adafruit_MotorHAT_setPin(motor->hat, motor->BIN1, LOW);
                Adafruit_MotorHAT_setPin(motor->hat, motor->BIN2, HIGH);
                break;
            case 2:
                Adafruit_MotorHAT_setPin(motor->hat, motor->AIN1, LOW);
                Adafruit_MotorHAT_setPin(motor->hat, motor->AIN2, HIGH);
                Adafruit_MotorHAT_setPin(motor->hat, motor->BIN1, HIGH);
                Adafruit_MotorHAT_setPin(motor->hat, motor->BIN2, LOW);
                break;
            case 3:
                Adafruit_MotorHAT_setPin(motor->hat, motor->AIN1, HIGH);
                Adafruit_MotorHAT_setPin(motor->hat, motor->AIN2, LOW);
                Adafruit_MotorHAT_setPin(motor->hat, motor->BIN1, HIGH);
                Adafruit_MotorHAT_setPin(motor->hat, motor->BIN2, LOW);
                break;
        }
    }
    
    return motor->currentstep;
}

// Step the motor for a given number of steps
void Adafruit_StepperMotor_step(Adafruit_StepperMotor *motor, int steps, Direction direction, Style stepstyle) {
    for (int i = 0; i < steps; i++) {
        Adafruit_StepperMotor_oneStep(motor, direction, stepstyle);
        usleep(motor->sec_per_step * 1000000);  // Convert seconds per step to microseconds
    }
}

// Initialize a DC motor
void Adafruit_DCMotor_init(Adafruit_DCMotor *motor, Adafruit_MotorHAT *hat, int num) {
    motor->hat = hat;
    motor->motornum = num;

    // Assign PWM and control pins based on motor number
    if (num == 1) {
        motor->PWM = 8;
        motor->IN2 = 9;
        motor->IN1 = 10;
    } else if (num == 2) {
        motor->PWM = 13;
        motor->IN2 = 12;
        motor->IN1 = 11;
    } else if (num == 3) {
        motor->PWM = 2;
        motor->IN2 = 3;
        motor->IN1 = 4;
    } else if (num == 4) {
        motor->PWM = 7;
        motor->IN2 = 6;
        motor->IN1 = 5;
    }
}

// Run the DC motor in a specific direction
void Adafruit_DCMotor_run(Adafruit_DCMotor *motor, Direction dir) {
    switch (dir) {
        case FORWARD:
            digitalWrite(motor->IN1, HIGH);
            digitalWrite(motor->IN2, LOW);
            break;
        case BACKWARD:
            digitalWrite(motor->IN1, LOW);
            digitalWrite(motor->IN2, HIGH);
            break;
        case BRAKE:
            digitalWrite(motor->IN1, HIGH);
            digitalWrite(motor->IN2, HIGH);
            break;
        case RELEASE:
            digitalWrite(motor->IN1, LOW);
            digitalWrite(motor->IN2, LOW);
            break;
    }
}

// Set the speed of the DC motor (0-255)
void Adafruit_DCMotor_setSpeed(Adafruit_DCMotor *motor, int speed) {
    softPwmCreate(motor->PWM, 0, 255);  // Initialize software PWM
    softPwmWrite(motor->PWM, speed);    // Set the speed
}

