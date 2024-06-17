#include "Adafruit_MotorHAT.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h> // for usleep

Adafruit_MotorHAT hat;

volatile sig_atomic_t keepRunning = 1;

void ctrl_c_handler(int s){
    printf("Caught signal %d\n", s);
    Adafruit_MotorHAT_resetAll(&hat); // Call the resetAll function with hat as an argument
    keepRunning = 0; // Set the flag to 0 to break the loop
}

int main(int argc, char** argv) {
    // Register the signal handler
    signal(SIGINT, ctrl_c_handler);

    // Initialize the Motor HAT
    Adafruit_MotorHAT_init(&hat, 0x60, 1600, -1, -1);

    // Get stepper motor instance from port #1
    Adafruit_StepperMotor* myStepper = Adafruit_MotorHAT_getStepper(&hat, 1);
    if (myStepper == NULL) {
        fprintf(stderr, "Failed to get stepper motor\n");
        return 1;
    }
    printf("Got stepper\n");

    // Set the speed of the stepper motor to 30 RPM
    Adafruit_StepperMotor_setSpeed(myStepper, 30);

    // Main loop for stepping the motor
    while (keepRunning) {
        printf("Single coil steps\n");
        Adafruit_StepperMotor_step(myStepper, 100, FORWARD, SINGLE);
        Adafruit_StepperMotor_step(myStepper, 100, BACKWARD, SINGLE);

        printf("Double coil steps\n");
        Adafruit_StepperMotor_step(myStepper, 100, FORWARD, DOUBLE);
        Adafruit_StepperMotor_step(myStepper, 100, BACKWARD, DOUBLE);

        printf("Interleaved coil steps\n");
        Adafruit_StepperMotor_step(myStepper, 100, FORWARD, INTERLEAVE);
        Adafruit_StepperMotor_step(myStepper, 100, BACKWARD, INTERLEAVE);

        printf("Microsteps\n");
        Adafruit_StepperMotor_step(myStepper, 100, FORWARD, MICROSTEP);
        Adafruit_StepperMotor_step(myStepper, 100, BACKWARD, MICROSTEP);

        usleep(100000); // Sleep for 100ms to prevent busy waiting
    }

    // Additional cleanup if necessary
    printf("Exiting program\n");
    Adafruit_MotorHAT_resetAll(&hat); // Call the resetAll function with hat as an argument

    return 0;
}