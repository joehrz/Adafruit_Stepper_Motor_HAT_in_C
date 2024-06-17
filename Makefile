# Compiler to use
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -I.

# Libraries to link
LIBS = -lwiringPi

# Target executable
TARGET = stepper_motor_test

# Source files
SRCS = Adafruit_MotorHAT.c PWM.c stepper_motor_test.c

# Object files
OBJS = $(SRCS:.c=.o)

# Default target to build
all: $(TARGET)

# Rule to build the target executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# Rule to build object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean target to remove generated files
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean