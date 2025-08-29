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
#include <SolarCalculator.h> // For sun data
#include <MoonRise.h> // For moon data

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

unsigned long lastStatusUpdate = 0;
unsigned long lastSerialOutput = 0;
unsigned long lastLogEntry = 0;
unsigned long gpsStartTime = 0;
bool gpsFixObtained = false;

// Set your geographic coordinates here
double MY_LATITUDE = 40.7128; // Example: New York City
double MY_LONGITUDE = -74.0060; // Example: New York City
int UTC_OFFSET_HOURS = -5; // For New York (Eastern Standard Time)
// Use an RTC to maintain accurate time

// Create objects for the libraries
MoonRise moonCalc;

void printDate(time_t date) {
  char buff[20];
  sprintf(buff, "%2d-%02d-%4d %02d:%02d:%02d", day(date), month(date), year(date), hour(date), minute(date), second(date));
  Serial.print(buff);
}
// A helper function to print times correctly
void printTime(double hours) {
  int h = (int)hours;
  int m = (int)((hours - h) * 60);
  int s = (int)((hours - h - m/60.0) * 3600);
  if (h < 10) Serial.print("0");
  Serial.print(h);
  Serial.print(":");
  if (m < 10) Serial.print("0");
  Serial.print(m);
  Serial.print(":");
  if (s < 10) Serial.print("0");
  Serial.print(s);
}

void testCalcs() {
    moonCalc.calculate(MY_LATITUDE, MY_LONGITUDE, now());

    time_t utc = now() - (UTC_OFFSET_HOURS * 3600); // SolarCalculator works with UTC

  // --- Sun Calculations ---
  double transit, sunrise, sunset;
  calcSunriseSunset(utc, MY_LATITUDE, MY_LONGITUDE, transit, sunrise, sunset);

  Serial.println("\n--- Sun Data ---");
  Serial.print("Current time: ");
  Serial.print(hour());
  Serial.print(":");
  Serial.print(minute());
  Serial.print(":");
  Serial.println(second());

  Serial.print("Sunrise: ");
  printTime(sunrise + UTC_OFFSET_HOURS);
  Serial.println();

  Serial.print("Sunset: ");
  printTime(sunset + UTC_OFFSET_HOURS);
  Serial.println();

  // --- Moon Calculations ---
  Serial.println("\n--- Moon Data ---");
  moonCalc.calculate(MY_LATITUDE, MY_LONGITUDE, now());
  
  if (moonCalc.hasRise) {
    Serial.print("Moon Rise: ");
    printDate(moonCalc.riseTime);
    Serial.print(" (Az: ");
    Serial.print(moonCalc.riseAz);
    Serial.println("°)");
  } else {
    Serial.println("Moon Rise: None today");
  }

  if (moonCalc.hasSet) {
    Serial.print("Moon Set: ");
    printDate(moonCalc.setTime);
    Serial.print(" (Az: ");
    Serial.print(moonCalc.setAz);
    Serial.println("°)");
  } else {
    Serial.println("Moon Set: None today");
  }

  Serial.print("Moon currently visible: ");
  Serial.println(moonCalc.isVisible ? "Yes" : "No");

}



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
    configManager->setConfiguration({ONCE_PER_MINUTE, "00:00", 0, false});
    configManager->begin();
    
    gpsManager = new GPSManager();
    gpsManager->begin();
    
    stepperController = new StepperController();
    stepperController->begin();
    
    displayManager = new DisplayManager();
    displayManager->begin();
    
    bluetoothManager = new BluetoothManager();
    bluetoothManager->begin();
    
    
    gpsStartTime = millis();
    
    stepperController->setRotationSpeed(ONCE_PER_MINUTE);
    stepperController->startRotation();
    
    logManager->logInfo("System startup completed");
    Serial.println("System initialization complete");

    // Initialize moon calculator with current time

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
            testCalcs();
        } else if ((currentTime - gpsStartTime) > 60 * 1000) { // 1 minutes timeout
            gpsFixObtained = true;
            // Use default values
            gpsManager->setDefaultLocation();
            logManager->logInfo("GPS timeout, using default location and build time");
        }
    }
    
    // Update stepper motor position
    stepperController->update();

    // Update OLED display every 1000ms
    if (currentTime - lastStatusUpdate > 1000) {
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

    testCalcs();
    return String(timeStr) + " [" + String(latDeg) + "' " + String(latMin) + ", " +
           String(lngDeg) + "' " + String(lngMin) + "] ";
}

String buildActivityText() {
    Configuration config = configManager->getConfiguration();
    String mode;
    
    switch(config.rotationSpeed) {
        case ONCE_PER_MINUTE: mode = "1m"; break;
        case ONCE_PER_HOUR: mode = "1h"; break;
        case ONCE_PER_DAY: mode = "1d"; break;
    }
    
    String result = "Mode: " + mode + " ";
    
    // Check if we're before start time or calculate remaining time
    if (configManager->isBeforeStartTime()) {
        int remainingMins = configManager->getMinutesUntilStart();
        int hours = remainingMins / 60;
        int mins = remainingMins % 60;
        result += "Begin in " + String(hours) + "h " + (mins < 10 ? "0" : "") + String(mins);
    } else if (configManager->isCompleted()) {
        result += "completed";
    } else {
        int remainingMins = configManager->getRemainingMinutes();
        int hours = remainingMins / 60;
        int mins = remainingMins % 60;
        result += "Remain " + String(hours) + "h " + (mins < 10 ? "0" : "") + String(mins);
    }
    
    return result;
}