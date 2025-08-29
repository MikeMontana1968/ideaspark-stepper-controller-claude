#include "StepperController.h"

StepperController::StepperController(uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4) {
    Serial.println("StepperController::StepperController()");
    stepper = nullptr;
    currentSpeed = ONCE_PER_DAY;
    lastStepTime = 0;
    stepsPerRevolution = 2048; // 28BYJ-48 with ULN2003
    currentSteps = 0;
    rotating = false;
    stepInterval = 0;
    this->pin1 = pin1;
    this->pin2 = pin2;
    this->pin3 = pin3;
    this->pin4 = pin4;
}

StepperController::~StepperController() {
    Serial.println("StepperController::~StepperController()");
    if (stepper) {
        delete stepper;
    }
}

void StepperController::begin() {
    Serial.println("StepperController::begin()");
    // ULN2003 connected to configurable pins
    stepper = new AccelStepper(AccelStepper::FULL4WIRE, pin1, pin2, pin3, pin4);
    stepper->setMaxSpeed(1000);
    stepper->setAcceleration(500);
    calculateStepInterval();
}

void StepperController::update() {
    
    if (rotating && stepper) {
        //Serial.println("StepperController::update()"); // Commented out - called frequently
        unsigned long currentTime = millis();
        if (currentTime - lastStepTime >= stepInterval) {
            
            stepper->move(1);
            stepper->run();
            currentSteps++;
            lastStepTime = currentTime;
            
            // Allow time for motor to complete step before releasing coils
            delay(50); // 50ms delay for motor to physically move
            releaseCoils();
            
            // Reset step count after full revolution
            if (currentSteps >= stepsPerRevolution) {
                Serial.println("StepperController::update() Completed 1 rotation");
                currentSteps = 0;
            }
        }
    }
}

void StepperController::setRotationSpeed(RotationSpeed speed) {
    Serial.print("StepperController::setRotationSpeed(");
    Serial.print(speed);
    Serial.println(")");
    currentSpeed = speed;
    calculateStepInterval();
}

void StepperController::startRotation() {
    Serial.println("StepperController::startRotation()");
    rotating = true;
}

void StepperController::stopRotation() {
    Serial.println("StepperController::stopRotation()");
    rotating = false;
    releaseCoils();
}

void StepperController::rewind() {
    Serial.println("StepperController::rewind()");
    if (stepper) {
        stepper->move(-currentSteps);
        while (stepper->distanceToGo() != 0) {
            stepper->run();
            yield(); // Allow other tasks to run
        }
        currentSteps = 0;
    }
}

float StepperController::getCurrentDegrees() {
    // Serial.println("StepperController::getCurrentDegrees()");
    float result = (float)currentSteps * 360.0 / (float)stepsPerRevolution;
    // Serial.print("StepperController::getCurrentDegrees() returning: ");
    // Serial.println(result);
    return result;
}

bool StepperController::isRotating() {
    // Serial.println("StepperController::isRotating()");
    Serial.print("StepperController::isRotating() returning: ");
    Serial.println(rotating);
    return rotating;
}

void StepperController::calculateStepInterval() {
    // Serial.println("StepperController::calculateStepInterval()");
    switch (currentSpeed) {
        case ONCE_PER_MINUTE:
            stepInterval = 60000.0 / stepsPerRevolution; // 1 minute in ms / steps
            break;
        case ONCE_PER_HOUR:
            stepInterval = 3600000.0 / stepsPerRevolution; // 1 hour in ms / steps
            break;
        case ONCE_PER_DAY:
            stepInterval = 86400000.0 / stepsPerRevolution; // 1 day in ms / steps
            break;
    }
    Serial.print("StepperController::calculateStepInterval() set to: ");
    Serial.println(stepInterval);
}

void StepperController::releaseCoils() {
    if (stepper) {
        stepper->disableOutputs();
    }
}

String StepperController::getPins() {
    return "Pin1: " + String(pin1) + ", Pin2: " + String(pin2) + ", Pin3: " + String(pin3) + ", Pin4: " + String(pin4);
}