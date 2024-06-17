#include "Adafruit_MotorHAT.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // for usleep

const unsigned char MICROSTEP_CURVE[9] = {0, 10, 23, 36, 49, 62, 75, 87, 100};
const int step2coils[8][4] = {
    {1, 0, 0, 0},
    {1, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 1},
    {0, 0, 0, 1},
    {1, 0, 0, 1}
};

void Adafruit_MotorHAT_init(Adafruit_MotorHAT *hat, int addr, int freq, int i2c, int i2c_bus) {
    hat->_frequency = freq;
    for (int i = 0; i < 2; i++) {
        Adafruit_StepperMotor_init(&hat->steppers[i], hat, i + 1, 200);
    }
    for (int i = 0; i < 4; i++) {
        Adafruit_DCMotor_init(&hat->dcs[i], hat, i + 1);
    }
}

void Adafruit_MotorHAT_setPin(Adafruit_MotorHAT *hat, int pin, int value) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, value);
}

Adafruit_StepperMotor* Adafruit_MotorHAT_getStepper(Adafruit_MotorHAT *hat, int num) {
    if (num < 1 || num > 2) return NULL;
    return &hat->steppers[num - 1];
}

Adafruit_DCMotor* Adafruit_MotorHAT_getDC(Adafruit_MotorHAT *hat, int num) {
    if (num < 1 || num > 4) return NULL;
    return &hat->dcs[num - 1];
}

void Adafruit_MotorHAT_resetAll(Adafruit_MotorHAT *hat) {
    for (int i = 0; i < 2; i++) {
        Adafruit_StepperMotor_setSpeed(&hat->steppers[i], 0);
    }
    for (int i = 0; i < 4; i++) {
        Adafruit_DCMotor_setSpeed(&hat->dcs[i], 0);
    }
}

void Adafruit_StepperMotor_init(Adafruit_StepperMotor *motor, Adafruit_MotorHAT *hat, int num, int steps) {
    motor->hat = hat;
    motor->motornum = num;
    motor->revsteps = steps;
    motor->currentstep = 0;
    motor->steppingcounter = 0;
    motor->sec_per_step = 0;
    // Assign pins based on the motor number (specific to the HAT design)
    if (num == 1) {
        motor->PWMA = 8;
        motor->AIN2 = 9;
        motor->AIN1 = 10;
        motor->PWMB = 13;
        motor->BIN2 = 12;
        motor->BIN1 = 11;
    } else {
        motor->PWMA = 2;
        motor->AIN2 = 3;
        motor->AIN1 = 4;
        motor->PWMB = 7;
        motor->BIN2 = 6;
        motor->BIN1 = 5;
    }
}

void Adafruit_StepperMotor_setSpeed(Adafruit_StepperMotor *motor, int rpm) {
    motor->sec_per_step = 60.0 / (motor->revsteps * rpm);
}

int Adafruit_StepperMotor_oneStep(Adafruit_StepperMotor *motor, Direction dir, Style style) {
    int steps_per_rev = motor->revsteps;
    int step = motor->currentstep;

    // Implement stepping logic based on style
    if (style == SINGLE) {
        step += (dir == FORWARD) ? 1 : -1;
    } else if (style == DOUBLE) {
        step += (dir == FORWARD) ? 2 : -2;
    } else if (style == INTERLEAVE) {
        step += (dir == FORWARD) ? 1 : -1;
    } else if (style == MICROSTEP) {
        step += (dir == FORWARD) ? 1 : -1;
        step %= (steps_per_rev * MICROSTEP);
    }
    motor->currentstep = step % steps_per_rev;

    // Apply step to motor coils
    for (int i = 0; i < 4; i++) {
        digitalWrite(motor->coils[i], step2coils[motor->currentstep % 8][i]);
    }

    return motor->currentstep;
}

void Adafruit_StepperMotor_step(Adafruit_StepperMotor *motor, int steps, Direction direction, Style stepstyle) {
    for (int i = 0; i < steps; i++) {
        Adafruit_StepperMotor_oneStep(motor, direction, stepstyle);
        usleep(motor->sec_per_step * 1000000);
    }
}

void Adafruit_DCMotor_init(Adafruit_DCMotor *motor, Adafruit_MotorHAT *hat, int num) {
    motor->hat = hat;
    motor->motornum = num;
    // Assign pins based on the motor number (specific to the HAT design)
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
    } else {
        motor->PWM = 7;
        motor->IN2 = 6;
        motor->IN1 = 5;
    }
}

void Adafruit_DCMotor_run(Adafruit_DCMotor *motor, Direction dir) {
    if (dir == FORWARD) {
        digitalWrite(motor->IN1, HIGH);
        digitalWrite(motor->IN2, LOW);
    } else if (dir == BACKWARD) {
        digitalWrite(motor->IN1, LOW);
        digitalWrite(motor->IN2, HIGH);
    } else if (dir == BRAKE) {
        digitalWrite(motor->IN1, HIGH);
        digitalWrite(motor->IN2, HIGH);
    } else if (dir == RELEASE) {
        digitalWrite(motor->IN1, LOW);
        digitalWrite(motor->IN2, LOW);
    }
}

void Adafruit_DCMotor_setSpeed(Adafruit_DCMotor *motor, int speed) {
    softPwmCreate(motor->PWM, 0, 100);
    softPwmWrite(motor->PWM, speed);
}