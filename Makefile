# Compiler to use
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -g -I. -MMD -MP

# Libraries to link (include math library -lm)
LIBS = -lwiringPi -lm

# Target executable
TARGET = stepper_motor_test

# Source files
SRCS = Adafruit_MotorHAT.c PWM.c stepper_motor_test.c

# Object files
OBJS = $(SRCS:.c=.o)

# Dependency files
DEPS = $(OBJS:.o=.d)

# Default target to build
all: $(TARGET)

# Rule to link the object files to create the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

# Rule to compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Include the dependency files
-include $(DEPS)

# Clean up build files
clean:
	rm -f $(OBJS) $(DEPS) $(TARGET)

# Phony targets to ensure that these targets are always run, even if files with these names exist
.PHONY: all clean


