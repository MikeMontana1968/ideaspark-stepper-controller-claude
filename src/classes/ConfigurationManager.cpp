#include "ConfigurationManager.h"
#include <TimeLib.h>

ConfigurationManager::ConfigurationManager() {
    Serial.println("ConfigurationManager::ConfigurationManager()");
    configLoaded = false;
    rotationStartTime = 0;
}

ConfigurationManager::~ConfigurationManager() {
    Serial.println("ConfigurationManager::~ConfigurationManager()");
}

void ConfigurationManager::begin() {
    Serial.println("ConfigurationManager::begin()");
    loadConfiguration();
    if (!configLoaded) {
        setDefaultConfiguration();
        saveConfiguration();
    }
}

Configuration ConfigurationManager::getConfiguration() {
    Serial.println("ConfigurationManager::getConfiguration()");
    Serial.print("ConfigurationManager::getConfiguration() returning speed: ");
    Serial.println(currentConfig.rotationSpeed);
    return currentConfig;
}

void ConfigurationManager::setConfiguration(const Configuration& config) {
    Serial.print("ConfigurationManager::setConfiguration(speed: ");
    Serial.print(config.rotationSpeed);
    Serial.println(")");
    currentConfig = config;
    saveConfiguration();
}

void ConfigurationManager::loadConfiguration() {
    Serial.println("ConfigurationManager::loadConfiguration()");
    
    if (!SPIFFS.exists("/schedule.json")) {
        Serial.println("Configuration file does not exist");
        configLoaded = false;
        return;
    }
    
    File file = SPIFFS.open("/schedule.json", "r");
    if (!file) {
        Serial.println("Failed to open configuration file");
        configLoaded = false;
        return;
    }
    
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, file);
    file.close();
    
    if (error) {
        Serial.println("Failed to parse configuration JSON");
        configLoaded = false;
        return;
    }
    
    currentConfig.rotationSpeed = (RotationSpeed)doc["rotationSpeed"].as<int>();
    currentConfig.startTime = doc["startTime"].as<String>();
    currentConfig.durationHours = doc["durationHours"].as<int>();
    currentConfig.rewindAfterComplete = doc["rewindAfterComplete"].as<bool>();
    
    configLoaded = true;
    Serial.println("Configuration loaded successfully");
}

void ConfigurationManager::saveConfiguration() {
    Serial.println("ConfigurationManager::saveConfiguration()");
    
    DynamicJsonDocument doc(1024);
    doc["rotationSpeed"] = (int)currentConfig.rotationSpeed;
    doc["startTime"] = currentConfig.startTime;
    doc["durationHours"] = currentConfig.durationHours;
    doc["rewindAfterComplete"] = currentConfig.rewindAfterComplete;
    
    File file = SPIFFS.open("/schedule.json", "w");
    if (!file) {
        Serial.println("Failed to create configuration file");
        return;
    }
    
    serializeJson(doc, file);
    file.close();
    Serial.println("Configuration saved successfully");
}

bool ConfigurationManager::isBeforeStartTime() {
    Serial.println("ConfigurationManager::isBeforeStartTime()");
    
    if (currentConfig.startTime == "00:00" || currentConfig.startTime.length() == 0) {
        Serial.print("ConfigurationManager::isBeforeStartTime() returning: ");
        Serial.println(false);
        return false; // Start immediately
    }
    
    int startMinutes = parseTimeToMinutes(currentConfig.startTime);
    int currentMinutes = getCurrentMinutes();
    
    bool result = currentMinutes < startMinutes;
    Serial.print("ConfigurationManager::isBeforeStartTime() returning: ");
    Serial.println(result);
    return result;
}

bool ConfigurationManager::isCompleted() {
    Serial.println("ConfigurationManager::isCompleted()");
    
    if (currentConfig.durationHours == 0) {
        Serial.print("ConfigurationManager::isCompleted() returning: ");
        Serial.println(false);
        return false; // Continuous rotation
    }
    
    if (rotationStartTime == 0 && !isBeforeStartTime()) {
        rotationStartTime = millis();
    }
    
    if (rotationStartTime == 0) {
        Serial.print("ConfigurationManager::isCompleted() returning: ");
        Serial.println(false);
        return false;
    }
    
    unsigned long elapsedMs = millis() - rotationStartTime;
    unsigned long durationMs = (unsigned long)currentConfig.durationHours * 3600000UL;
    
    bool result = elapsedMs >= durationMs;
    Serial.print("ConfigurationManager::isCompleted() returning: ");
    Serial.println(result);
    return result;
}

int ConfigurationManager::getMinutesUntilStart() {
    Serial.println("ConfigurationManager::getMinutesUntilStart()");
    
    int startMinutes = parseTimeToMinutes(currentConfig.startTime);
    int currentMinutes = getCurrentMinutes();
    
    int result = startMinutes - currentMinutes;
    if (result < 0) {
        result += 24 * 60; // Add a day
    }
    
    Serial.print("ConfigurationManager::getMinutesUntilStart() returning: ");
    Serial.println(result);
    return result;
}

int ConfigurationManager::getRemainingMinutes() {
    Serial.println("ConfigurationManager::getRemainingMinutes()");
    
    if (currentConfig.durationHours == 0) {
        Serial.print("ConfigurationManager::getRemainingMinutes() returning: ");
        Serial.println(999999);
        return 999999; // Continuous
    }
    
    if (rotationStartTime == 0) {
        Serial.print("ConfigurationManager::getRemainingMinutes() returning: ");
        Serial.println(currentConfig.durationHours * 60);
        return currentConfig.durationHours * 60;
    }
    
    unsigned long elapsedMs = millis() - rotationStartTime;
    unsigned long durationMs = (unsigned long)currentConfig.durationHours * 3600000UL;
    
    if (elapsedMs >= durationMs) {
        Serial.print("ConfigurationManager::getRemainingMinutes() returning: ");
        Serial.println(0);
        return 0;
    }
    
    unsigned long remainingMs = durationMs - elapsedMs;
    int result = remainingMs / 60000;
    
    Serial.print("ConfigurationManager::getRemainingMinutes() returning: ");
    Serial.println(result);
    return result;
}

void ConfigurationManager::setDefaultConfiguration() {
    Serial.println("ConfigurationManager::setDefaultConfiguration()");
    currentConfig.rotationSpeed = ONCE_PER_DAY;
    currentConfig.startTime = "00:00";
    currentConfig.durationHours = 0;
    currentConfig.rewindAfterComplete = false;
    configLoaded = true;
}

int ConfigurationManager::parseTimeToMinutes(const String& timeStr) {
    Serial.print("ConfigurationManager::parseTimeToMinutes(");
    Serial.print(timeStr);
    Serial.println(")");
    
    int colonIndex = timeStr.indexOf(':');
    if (colonIndex == -1) {
        Serial.print("ConfigurationManager::parseTimeToMinutes() returning: ");
        Serial.println(0);
        return 0;
    }
    
    int hours = timeStr.substring(0, colonIndex).toInt();
    int minutes = timeStr.substring(colonIndex + 1).toInt();
    
    int result = hours * 60 + minutes;
    Serial.print("ConfigurationManager::parseTimeToMinutes() returning: ");
    Serial.println(result);
    return result;
}

int ConfigurationManager::getCurrentMinutes() {
    Serial.println("ConfigurationManager::getCurrentMinutes()");
    int result = hour() * 60 + minute();
    Serial.print("ConfigurationManager::getCurrentMinutes() returning: ");
    Serial.println(result);
    return result;
}