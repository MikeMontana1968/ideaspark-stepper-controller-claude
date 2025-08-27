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
    StepperController();
    ~StepperController();
    
    void begin();
    void update();
    void setRotationSpeed(RotationSpeed speed);
    void startRotation();
    void stopRotation();
    void rewind();
    float getCurrentDegrees();
    bool isRotating();

private:
    AccelStepper* stepper;
    RotationSpeed currentSpeed;
    unsigned long lastStepTime;
    long stepsPerRevolution;
    long currentSteps;
    bool rotating;
    float stepInterval; // milliseconds between steps
    
    void calculateStepInterval();
};

#endif