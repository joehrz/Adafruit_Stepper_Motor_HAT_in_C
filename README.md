# Adafruit Motor HAT C Library for Raspberry Pi

This project provides a C library for controlling stepper and DC motors using the [Adafruit Motor HAT](https://www.adafruit.com/product/2348) on a Raspberry Pi. The library interfaces with the PCA9685 PWM controller over I2C to control motors connected to the Motor HAT.

## Features

- **Control Multiple Motors**: Supports up to 2 stepper motors and 4 DC motors simultaneously.
- **Stepping Styles**: Implements SINGLE, DOUBLE, INTERLEAVE, and MICROSTEP stepping modes for precise control.
- **Easy-to-Use API**: Simple functions to initialize and control motors.
- **Test Program**: Includes `stepper_motor_test.c` to demonstrate how to use the library.

## Prerequisites

### Hardware

- **Raspberry Pi**: Any model with I2C support (e.g., Raspberry Pi 3, 4, Zero).
- **Adafruit Motor HAT**: [Product Link](https://www.adafruit.com/product/2348).
- **Stepper Motor**: Compatible with the Motor HAT (e.g., NEMA 17 stepper motor).
- **Power Supply**: External power supply for the motors (required for driving stepper motors).
- **Wiring**: Jumper wires and connectors as needed.

### Software

- **Operating System**: Raspbian OS (or any Linux distribution for Raspberry Pi).
- **Compiler**: GCC compiler (`build-essential` package).
- **I2C Tools**: Install with `sudo apt-get install i2c-tools`.
- **I2C Library**:
  - **Option 1**: `wiringPi` library (deprecated but still usable).
  - **Option 2**: Alternative libraries like `pigpio` or direct I2C communication.

## Installation Instructions

### 1. Clone the Repository

```bash
git clone https://github.com/Dxxc/Adafruit_Stepper_Motor_HAT_in_C.git
cd Adafruit_MotorHAT_C_Library

