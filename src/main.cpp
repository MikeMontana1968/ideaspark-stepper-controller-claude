#include <Arduino.h>
#include <SPIFFS.h>
#include <TimeLib.h>
#include <Time.h>
#include "classes/GPSManager.h"
#include "classes/StepperController.h"
#include "classes/DisplayManager.h"
#include "classes/BluetoothManager.h"
#include "classes/ConfigurationManager.h"
#include "classes/LogManager.h"
#include "classes/EphemerisCalculator.h"

const String PROMPT_VERSION = "Prompt Document Version 1.0.2";

// Forward function declarations
void setup();
void loop();
String buildStatusText();
String buildActivityText();

GPSManager* gpsManager;
StepperController* stepperController;
DisplayManager* displayManager;
BluetoothManager* bluetoothManager;
ConfigurationManager* configManager;
LogManager* logManager;
EphemerisCalculator* ephemerisCalc;

unsigned long lastStatusUpdate = 0;
unsigned long lastSerialOutput = 0;
unsigned long lastLogEntry = 0;
unsigned long gpsStartTime = 0;
bool gpsFixObtained = false;

void setup() {
    Serial.begin(115200);
    while (!Serial) {
        delay(10);
    }
    
    Serial.print("Starting… ");
    Serial.print(__DATE__);
    Serial.print(" ");
    Serial.print(__TIME__);
    Serial.println();
    Serial.println(PROMPT_VERSION);
    
    // Initialize SPIFFS
    if (!SPIFFS.begin(true)) {
        Serial.println("SPIFFS Mount Failed");
        return;
    }
    
    // Initialize managers
    logManager = new LogManager();
    logManager->begin();
    logManager->logInfo("System startup initiated");
    
    configManager = new ConfigurationManager();
    configManager->begin();
    
    gpsManager = new GPSManager();
    gpsManager->begin();
    
    stepperController = new StepperController();
    stepperController->begin();
    
    displayManager = new DisplayManager();
    displayManager->begin();
    
    bluetoothManager = new BluetoothManager();
    bluetoothManager->begin();
    
    ephemerisCalc = new EphemerisCalculator();
    
    gpsStartTime = millis();
    
    logManager->logInfo("System startup completed");
    Serial.println("System initialization complete");
}

void loop() {
    unsigned long currentTime = millis();
    
    // Poll Bluetooth for user interaction
    bluetoothManager->handleUserInteraction();
    
    // Update GPS data
    gpsManager->update();
    
    // Check for GPS fix or timeout
    if (!gpsFixObtained) {
        if (gpsManager->hasValidFix()) {
            gpsFixObtained = true;
            // Set system time from GPS
            setTime(gpsManager->getHour(), gpsManager->getMinute(), gpsManager->getSecond(),
                   gpsManager->getDay(), gpsManager->getMonth(), gpsManager->getYear());
            logManager->logInfo("GPS fix obtained, system time set");
        } else if ((currentTime - gpsStartTime) > 600000) { // 10 minutes timeout
            gpsFixObtained = true;
            // Use default values
            gpsManager->setDefaultLocation();
            logManager->logInfo("GPS timeout, using default location and build time");
        }
    }
    
    // Update stepper motor position
    stepperController->update();
    
    // Update OLED display every 250ms
    if (currentTime - lastStatusUpdate > 250) {
        String statusText = buildStatusText();
        String activityText = buildActivityText();
        displayManager->updateDisplay(statusText, activityText);
        lastStatusUpdate = currentTime;
    }
    
    // Update serial port every 5 seconds
    if (currentTime - lastSerialOutput > 5000) {
        Serial.println(buildStatusText());
        lastSerialOutput = currentTime;
    }
    
    // Log entry every 1 minute
    if (currentTime - lastLogEntry > 60000) {
        String logEntry = buildStatusText() + " | " + buildActivityText();
        logManager->logInfo(logEntry);
        lastLogEntry = currentTime;
    }
}

String buildStatusText() {
    if (!gpsFixObtained) {
        unsigned long uptime = (millis() - gpsStartTime) / 1000;
        int minutes = uptime / 60;
        int seconds = uptime % 60;
        return "Waiting for GPS, " + String(minutes) + ":" + 
               (seconds < 10 ? "0" : "") + String(seconds);
    }
    
    char timeStr[9];
    sprintf(timeStr, "%02d:%02d:%02d", hour(), minute(), second());
    
    float degrees = stepperController->getCurrentDegrees();
    float lat = gpsManager->getLatitude();
    float lng = gpsManager->getLongitude();
    
    int latDeg = (int)lat;
    int latMin = (int)((lat - latDeg) * 60);
    int lngDeg = (int)lng;
    int lngMin = (int)((lng - lngDeg) * 60);
    
    return String(timeStr) + " " + String(degrees, 1) + "° " + 
           String(latDeg) + " " + String(latMin) + ", " + 
           String(lngDeg) + " " + String(lngMin) + " ";
}

String buildActivityText() {
    Configuration config = configManager->getConfiguration();
    String mode;
    
    switch(config.rotationSpeed) {
        case ONCE_PER_MINUTE: mode = "1m"; break;
        case ONCE_PER_HOUR: mode = "1h"; break;
        case ONCE_PER_DAY: mode = "1d"; break;
    }
    
    // Calculate moon setting compass heading
    int moonHeading = ephemerisCalc->getMoonSettingHeading(
        gpsManager->getLatitude(), gpsManager->getLongitude());
    
    String result = "Mode: " + mode + " " + String(moonHeading) + "° ";
    
    // Check if we're before start time or calculate remaining time
    if (configManager->isBeforeStartTime()) {
        int remainingMins = configManager->getMinutesUntilStart();
        int hours = remainingMins / 60;
        int mins = remainingMins % 60;
        result += "Begin in " + String(hours) + ":" + (mins < 10 ? "0" : "") + String(mins);
    } else if (configManager->isCompleted()) {
        result += "completed";
    } else {
        int remainingMins = configManager->getRemainingMinutes();
        int hours = remainingMins / 60;
        int mins = remainingMins % 60;
        result += "Remain " + String(hours) + ":" + (mins < 10 ? "0" : "") + String(mins);
    }
    
    return result;
}