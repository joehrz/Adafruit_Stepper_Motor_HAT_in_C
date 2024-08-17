#ifndef ADAFRUIT_MOTORHAT_H
#define ADAFRUIT_MOTORHAT_H

// Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "PWM.h"  // Include PWM.h to get the definition of PWM

// Definitions for motor stepping
#define MICROSTEPS 16

// Direction enum
typedef enum {
    FORWARD,
    BACKWARD,
    RELEASE
} Direction;

// Step style enum
typedef enum {
    SINGLE,
    DOUBLE,
    INTERLEAVE,
    MICROSTEP
} Style;

// Forward declaration of Adafruit_MotorHAT
typedef struct Adafruit_MotorHAT Adafruit_MotorHAT;

// Adafruit_StepperMotor structure
typedef struct {
    Adafruit_MotorHAT *hat;
    int revsteps;
    int motornum;
    float sec_per_step;
    int steppingcounter;
    int currentstep;
    int PWMA, AIN2, AIN1, PWMB, BIN2, BIN1;
    int coils[4];
} Adafruit_StepperMotor;

// Adafruit_DCMotor structure
typedef struct {
    Adafruit_MotorHAT *hat;
    int motornum;
    int PWM, IN1, IN2;
} Adafruit_DCMotor;

// Adafruit_MotorHAT structure
struct Adafruit_MotorHAT {
    int _frequency;
    PWM pwmController;  // Add this line to include the PWM controller
    Adafruit_StepperMotor steppers[2];
    Adafruit_DCMotor dcs[4];
};

// Function declarations
void Adafruit_MotorHAT_init(Adafruit_MotorHAT *hat, int addr, int freq);
void Adafruit_MotorHAT_resetAll(Adafruit_MotorHAT *hat);
void Adafruit_MotorHAT_setPin(Adafruit_MotorHAT *hat, int pin, int value);

void Adafruit_StepperMotor_init(Adafruit_StepperMotor *motor, Adafruit_MotorHAT *hat, int num, int steps);
void Adafruit_StepperMotor_setSpeed(Adafruit_StepperMotor *motor, int rpm);
int Adafruit_StepperMotor_oneStep(Adafruit_StepperMotor *motor, Direction dir, Style style);
void Adafruit_StepperMotor_step(Adafruit_StepperMotor *motor, int steps, Direction direction, Style stepstyle);

void Adafruit_DCMotor_init(Adafruit_DCMotor *motor, Adafruit_MotorHAT *hat, int num);
void Adafruit_DCMotor_run(Adafruit_DCMotor *motor, Direction dir);
void Adafruit_DCMotor_setSpeed(Adafruit_DCMotor *motor, int speed);

#endif // ADAFRUIT_MOTORHAT_H


