#ifndef CONFIGURATION_MANAGER_H
#define CONFIGURATION_MANAGER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include "StepperController.h"

struct Configuration {
    RotationSpeed rotationSpeed;
    String startTime;
    int durationHours;
    bool rewindAfterComplete;
};

class ConfigurationManager {
public:
    ConfigurationManager();
    ~ConfigurationManager();
    
    void begin();
    Configuration getConfiguration();
    void setConfiguration(const Configuration& config);
    void loadConfiguration();
    void saveConfiguration();
    bool isBeforeStartTime();
    bool isCompleted();
    int getMinutesUntilStart();
    int getRemainingMinutes();

private:
    Configuration currentConfig;
    bool configLoaded;
    unsigned long rotationStartTime;
    
    void setDefaultConfiguration();
    int parseTimeToMinutes(const String& timeStr);
    int getCurrentMinutes();
};

#endif