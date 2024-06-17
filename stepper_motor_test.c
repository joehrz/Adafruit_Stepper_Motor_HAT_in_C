#include "Adafruit_MotorHAT.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h> // for usleep

Adafruit_MotorHAT hat;

volatile sig_atomic_t keepRunning = 1;

void ctrl_c_handler(int s){
    printf("Caught signal %d\n", s);
    hat.resetAll();
    keepRunning = 0; // Set the flag to 0 to break the loop
}

int main(int argc, char** argv) {
    // Register the signal handler
    signal(SIGINT, ctrl_c_handler);

    // Get stepper motor instance from port #1
    Adafruit_StepperMotor* myStepper = hat.getStepper(&hat, 1);
    if (myStepper == NULL) {
        fprintf(stderr, "Failed to get stepper motor\n");
        return 1;
    }
    printf("Got stepper\n");

    // Set the speed of the stepper motor to 30 RPM
    myStepper->setSpeed(myStepper, 30);

    // Main loop for stepping the motor
    while (keepRunning) {
        printf("Single coil steps\n");
        myStepper->step(myStepper, 100, FORWARD, SINGLE);
        myStepper->step(myStepper, 100, BACKWARD, SINGLE);

        printf("Double coil steps\n");
        myStepper->step(myStepper, 100, FORWARD, DOUBLE);
        myStepper->step(myStepper, 100, BACKWARD, DOUBLE);

        printf("Interleaved coil steps\n");
        myStepper->step(myStepper, 100, FORWARD, INTERLEAVE);
        myStepper->step(myStepper, 100, BACKWARD, INTERLEAVE);

        printf("Microsteps\n");
        myStepper->step(myStepper, 100, FORWARD, MICROSTEP);
        myStepper->step(myStepper, 100, BACKWARD, MICROSTEP);

        usleep(100000); // Sleep for 100ms to prevent busy waiting
    }

    // Additional cleanup if necessary
    printf("Exiting program\n");
    hat.resetAll();

    return 0;
}