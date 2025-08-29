#ifndef STEPPER_CONTROLLER_H
#define STEPPER_CONTROLLER_H

#include <Arduino.h>
#include <AccelStepper.h>

enum RotationSpeed {
    ONCE_PER_MINUTE = 0,
    ONCE_PER_HOUR = 1,
    ONCE_PER_DAY = 2
};

class StepperController {
public:
    StepperController(uint8_t pin1 = 25, uint8_t pin2 = 26, uint8_t pin3 = 27, uint8_t pin4 = 14);
    ~StepperController();
    
    void begin();
    void update();
    void setRotationSpeed(RotationSpeed speed);
    void startRotation();
    void stopRotation();
    void rewind();
    float getCurrentDegrees();
    bool isRotating();
    void releaseCoils();
    String getPins();

private:
    AccelStepper* stepper;
    RotationSpeed currentSpeed;
    unsigned long lastStepTime;
    long stepsPerRevolution;
    long currentSteps;
    bool rotating;
    float stepInterval; // milliseconds between steps
    uint8_t pin1, pin2, pin3, pin4; // GPIO pins for stepper motor
    
    void calculateStepInterval();
};

#endif