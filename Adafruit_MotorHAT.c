#include "Adafruit_MotorHAT.h"
#include "PWM.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// Function to initialize the Motor HAT
void Adafruit_MotorHAT_init(Adafruit_MotorHAT *hat, int addr, int freq) {
    hat->_frequency = freq;

    // Initialize PWM
    PWM_init(&(hat->pwmController), addr);
    PWM_setPWMFreq(&(hat->pwmController), hat->_frequency);

    // Initialize steppers
    for (int idx = 0; idx < 2; idx++) {
        Adafruit_StepperMotor_init(&(hat->steppers[idx]), hat, idx + 1, 200);
    }

    // Initialize DC motors
    for (int idx = 0; idx < 4; idx++) {
        Adafruit_DCMotor_init(&(hat->dcs[idx]), hat, idx + 1);  // idx + 1 to match motor numbers 1-4
    }
}


// Function to reset all motors
void Adafruit_MotorHAT_resetAll(Adafruit_MotorHAT *hat) {
    PWM_setAllPWM(&(hat->pwmController), 0, 0);  // Reset all PWM channels to off
}

// Set a pin to a high or low value
void Adafruit_MotorHAT_setPin(Adafruit_MotorHAT *hat, int pin, int value) {
    if (pin < 0 || pin > 15) {
        printf("Error: PWM pin must be between 0 and 15 inclusive\n");
        return;
    }
    if (value != 0 && value != 1) {
        printf("Error: Pin value must be 0 or 1!\n");
        return;
    }

    if (value == 0) {
        PWM_setPWM(&(hat->pwmController), pin, 0, 4096);  // Turn the pin off
    } else {
        PWM_setPWM(&(hat->pwmController), pin, 4096, 0);  // Turn the pin on
    }
}

// Initialize a stepper motor
void Adafruit_StepperMotor_init(Adafruit_StepperMotor *motor, Adafruit_MotorHAT *hat, int num, int steps) {
    motor->hat = hat;
    motor->revsteps = steps;
    motor->motornum = num;
    motor->sec_per_step = 0.1;
    motor->steppingcounter = 0;
    motor->currentstep = 0;

    num -= 1;
    if (num == 0) {
        motor->PWMA = 8;
        motor->AIN2 = 9;
        motor->AIN1 = 10;
        motor->PWMB = 13;
        motor->BIN2 = 12;
        motor->BIN1 = 11;
    } else if (num == 1) {
        motor->PWMA = 2;
        motor->AIN2 = 3;
        motor->AIN1 = 4;
        motor->PWMB = 7;
        motor->BIN2 = 6;
        motor->BIN1 = 5;
    } else {
        printf("Error: MotorHAT Stepper must be between 1 and 2 inclusive\n");
    }
}

// Set the speed of the stepper motor
void Adafruit_StepperMotor_setSpeed(Adafruit_StepperMotor *motor, int rpm) {
    motor->sec_per_step = 60.0 / (motor->revsteps * rpm);  // Calculate time per step
    motor->steppingcounter = 0;
}

// Perform one step of the stepper motor
int Adafruit_StepperMotor_oneStep(Adafruit_StepperMotor *motor, Direction dir, Style style) {
    unsigned char pwm_a = 255;
    unsigned char pwm_b = 255;

    // Adjust current step based on direction and style
    if (style == SINGLE) {
        if ((motor->currentstep / (MICROSTEPS / 2)) % 2 == 0) {  // Even steps
            // Step half a microstep
            motor->currentstep += (dir == FORWARD) ? MICROSTEPS / 2 : -MICROSTEPS / 2;
        } else {  // Odd steps
            // Step a full microstep
            motor->currentstep += (dir == FORWARD) ? MICROSTEPS : -MICROSTEPS;
        }
    } else if (style == DOUBLE) {
        motor->currentstep += (dir == FORWARD) ? MICROSTEPS : -MICROSTEPS;
    } else if (style == INTERLEAVE) {
        motor->currentstep += (dir == FORWARD) ? MICROSTEPS / 2 : -MICROSTEPS / 2;
    } else if (style == MICROSTEP) {
        motor->currentstep += (dir == FORWARD) ? 1 : -1;
        // Clamp currentstep within 0 to MICROSTEPS*4
        motor->currentstep += MICROSTEPS * 4;
        motor->currentstep %= MICROSTEPS * 4;

        pwm_a = pwm_b = 0;  // Microstepping requires variable PWM
    }

    // Wrap currentstep around
    motor->currentstep += MICROSTEPS * 4;
    motor->currentstep %= MICROSTEPS * 4;

    // Set coils based on currentstep
    int step_phase = motor->currentstep % (MICROSTEPS * 4);
    // Define the coil activation sequences
    static const int step_coils[8][4] = {
        {1, 0, 0, 0},  // Step 0
        {1, 1, 0, 0},  // Step 1
        {0, 1, 0, 0},  // Step 2
        {0, 1, 1, 0},  // Step 3
        {0, 0, 1, 0},  // Step 4
        {0, 0, 1, 1},  // Step 5
        {0, 0, 0, 1},  // Step 6
        {1, 0, 0, 1}   // Step 7
    };
    int coil_index = step_phase / (MICROSTEPS / 2);
    for (int i = 0; i < 4; i++) {
        motor->coils[i] = step_coils[coil_index][i];
    }

    // Set PWM outputs
    PWM_setPWM(&(motor->hat->pwmController), motor->PWMA, 0, pwm_a * 16);
    PWM_setPWM(&(motor->hat->pwmController), motor->PWMB, 0, pwm_b * 16);

    // Activate coils
    Adafruit_MotorHAT_setPin(motor->hat, motor->AIN2, motor->coils[0]);
    Adafruit_MotorHAT_setPin(motor->hat, motor->BIN1, motor->coils[1]);
    Adafruit_MotorHAT_setPin(motor->hat, motor->AIN1, motor->coils[2]);
    Adafruit_MotorHAT_setPin(motor->hat, motor->BIN2, motor->coils[3]);

    return motor->currentstep;
}


// Step the stepper motor a given number of steps
void Adafruit_StepperMotor_step(Adafruit_StepperMotor *motor, int steps, Direction direction, Style stepstyle) {
    float s_per_s = motor->sec_per_step;
    int lateststep = 0;

    if (stepstyle == INTERLEAVE) {
        s_per_s /= 2.0;
    } else if (stepstyle == MICROSTEP) {
        s_per_s /= MICROSTEPS;
        steps *= MICROSTEPS;
    }

    for (int s = 0; s < steps; s++) {
        lateststep = Adafruit_StepperMotor_oneStep(motor, direction, stepstyle);
        usleep((useconds_t)(s_per_s * 1e6));  // This is where you're using the line
    }

    if (stepstyle == MICROSTEP) {
        while (lateststep != 0 && lateststep != MICROSTEPS) {
            lateststep = Adafruit_StepperMotor_oneStep(motor, direction, stepstyle);
            usleep((useconds_t)(s_per_s * 1e6));  // Again, used here for microstepping
        }
    }
}


// Initialize a DC motor
void Adafruit_DCMotor_init(Adafruit_DCMotor *motor, Adafruit_MotorHAT *hat, int num) {
    motor->hat = hat;
    motor->motornum = num;

    if (num == 1) {
        motor->PWM = 8;
        motor->IN1 = 9;
        motor->IN2 = 10;
    } else if (num == 2) {
        motor->PWM = 13;
        motor->IN1 = 12;
        motor->IN2 = 11;
    } else if (num == 3) {
        motor->PWM = 2;
        motor->IN1 = 3;
        motor->IN2 = 4;
    } else if (num == 4) {
        motor->PWM = 7;
        motor->IN1 = 6;
        motor->IN2 = 5;
    } else {
        printf("Error: MotorHAT DC Motor must be between 1 and 4 inclusive\n");
    }
}


// Run the DC motor in the specified direction
void Adafruit_DCMotor_run(Adafruit_DCMotor *motor, Direction dir) {
    // PWM pwmController;

    if (dir == FORWARD) {
        Adafruit_MotorHAT_setPin(motor->hat, motor->IN2, 0);
        Adafruit_MotorHAT_setPin(motor->hat, motor->IN1, 1);
    } else if (dir == BACKWARD) {
        Adafruit_MotorHAT_setPin(motor->hat, motor->IN1, 0);
        Adafruit_MotorHAT_setPin(motor->hat, motor->IN2, 1);
    } else if (dir == RELEASE) {
        Adafruit_MotorHAT_setPin(motor->hat, motor->IN1, 0);
        Adafruit_MotorHAT_setPin(motor->hat, motor->IN2, 0);
    }
}

// Set the speed of the DC motor
void Adafruit_DCMotor_setSpeed(Adafruit_DCMotor *motor, int speed) {
    if (speed < 0) {
        speed = 0;
    } else if (speed > 255) {
        speed = 255;
    }

    PWM_setPWM(&(motor->hat->pwmController), motor->PWM, 0, speed * 16);
}





