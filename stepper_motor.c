#include "Adafruit_MotorHAT.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h> // for usleep

// Declare the global Motor HAT object
Adafruit_MotorHAT hat;

// Flag to handle Ctrl+C signal for a clean exit
volatile sig_atomic_t keepRunning = 1;

// Signal handler to catch Ctrl+C
void ctrl_c_handler(int s) {
    printf("Caught signal %d\n", s);
    Adafruit_MotorHAT_resetAll(&hat); // Reset all motors before exit
    keepRunning = 0; // Set the flag to stop the loop
}

int main() {
    // Register the signal handler for Ctrl+C (SIGINT)
    signal(SIGINT, ctrl_c_handler);

    // Initialize the Motor HAT at I2C address 0x60 and PWM frequency 1600
    Adafruit_MotorHAT_init(&hat, 0x60, 1526);

    // Get the stepper motor instance from port #1
    Adafruit_StepperMotor* myStepper = &(hat.steppers[0]);

    printf("Got stepper motor instance\n");

    // Set the speed of the stepper motor to 30 RPM
    Adafruit_StepperMotor_setSpeed(myStepper, 30);

    // Optional: Loop for continuous motor movement until Ctrl+C
    while (keepRunning) {
        printf("Moving motor forward 10 steps (SINGLE step mode)\n");
        Adafruit_StepperMotor_step(myStepper, 10, FORWARD, SINGLE);
        
        usleep(500000);  // Sleep for 500ms between movements

        printf("Moving motor backward 10 steps (SINGLE step mode)\n");
        Adafruit_StepperMotor_step(myStepper, 10, BACKWARD, SINGLE);
        
        usleep(500000);  // Sleep for 500ms between movements
    }

    // Additional cleanup if necessary
    printf("Exiting program\n");
    Adafruit_MotorHAT_resetAll(&hat); // Reset all motors before exiting

    return 0;
}






