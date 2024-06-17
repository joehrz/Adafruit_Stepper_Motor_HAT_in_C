#ifndef ADAFRUIT_MOTORHAT_H
#define ADAFRUIT_MOTORHAT_H

#include <wiringPi.h>
#include <softPwm.h>

// Define enums for direction and style
typedef enum {
    FORWARD,
    BACKWARD,
    BRAKE,
    RELEASE
} Direction;

typedef enum {
    SINGLE,
    DOUBLE,
    INTERLEAVE,
    MICROSTEP
} Style;

typedef struct {
    int PWMA;
    int AIN2;
    int AIN1;
    int PWMB;
    int BIN2;
    int BIN1;
    int coils[4];
    int revsteps;
    int motornum;
    float sec_per_step;
    int steppingcounter;
    int currentstep;
} Adafruit_StepperMotor;

typedef struct {
    int PWM;
    int IN2;
    int IN1;
    int motornum;
} Adafruit_DCMotor;

typedef struct {
    int _frequency;
    Adafruit_StepperMotor steppers[2];
    Adafruit_DCMotor dcs[4];
} Adafruit_MotorHAT;

// Function prototypes
void Adafruit_MotorHAT_init(Adafruit_MotorHAT *hat, int addr, int freq, int i2c, int i2c_bus);
void Adafruit_MotorHAT_setPin(Adafruit_MotorHAT *hat, int pin, int value);
Adafruit_StepperMotor* Adafruit_MotorHAT_getStepper(Adafruit_MotorHAT *hat, int num);
Adafruit_DCMotor* Adafruit_MotorHAT_getDC(Adafruit_MotorHAT *hat, int num);
void Adafruit_MotorHAT_resetAll(Adafruit_MotorHAT *hat);

void Adafruit_StepperMotor_init(Adafruit_StepperMotor *motor, Adafruit_MotorHAT *hat, int num, int steps);
void Adafruit_StepperMotor_setSpeed(Adafruit_StepperMotor *motor, int rpm);
int Adafruit_StepperMotor_oneStep(Adafruit_StepperMotor *motor, Direction dir, Style style);
void Adafruit_StepperMotor_step(Adafruit_StepperMotor *motor, int steps, Direction direction, Style stepstyle);

void Adafruit_DCMotor_init(Adafruit_DCMotor *motor, Adafruit_MotorHAT *hat, int num);
void Adafruit_DCMotor_run(Adafruit_DCMotor *motor, Direction dir);
void Adafruit_DCMotor_setSpeed(Adafruit_DCMotor *motor, int speed);

#endif // ADAFRUIT_MOTORHAT_H