#include "GPSManager.h"

GPSManager::GPSManager() {
    Serial.println("GPSManager::GPSManager()");
    useDefaults = false;
    gpsSerial = nullptr;
}

GPSManager::~GPSManager() {
    Serial.println("GPSManager::~GPSManager()");
    if (gpsSerial) {
        delete gpsSerial;
    }
}

void GPSManager::begin() {
    Serial.println("GPSManager::begin()");
    // GPS connected to pins 16 (RX) and 17 (TX)
    gpsSerial = new SoftwareSerial(16, 17);
    gpsSerial->begin(9600);
}

void GPSManager::update() {
    // Serial.println("GPSManager::update()"); // Commented out - called frequently
    if (gpsSerial && gpsSerial->available()) {
        while (gpsSerial->available()) {
            if (gps.encode(gpsSerial->read())) {
                // GPS data updated
            }
        }
    }
}

bool GPSManager::hasValidFix() {
    Serial.println("GPSManager::hasValidFix()");
    bool result = !useDefaults && gps.location.isValid() && gps.date.isValid() && gps.time.isValid();
    Serial.print("GPSManager::hasValidFix() returning: ");
    Serial.println(result);
    return result;
}

void GPSManager::setDefaultLocation() {
    Serial.println("GPSManager::setDefaultLocation()");
    useDefaults = true;
}

float GPSManager::getLatitude() {
    Serial.println("GPSManager::getLatitude()");
    float result = useDefaults ? defaultLat : (float)gps.location.lat();
    Serial.print("GPSManager::getLatitude() returning: ");
    Serial.println(result);
    return result;
}

float GPSManager::getLongitude() {
    Serial.println("GPSManager::getLongitude()");
    float result = useDefaults ? defaultLng : (float)gps.location.lng();
    Serial.print("GPSManager::getLongitude() returning: ");
    Serial.println(result);
    return result;
}

float GPSManager::getAltitude() {
    Serial.println("GPSManager::getAltitude()");
    float result = useDefaults ? defaultAlt : (float)gps.altitude.meters();
    Serial.print("GPSManager::getAltitude() returning: ");
    Serial.println(result);
    return result;
}

int GPSManager::getYear() {
    Serial.println("GPSManager::getYear()");
    int result = useDefaults ? 2025 : gps.date.year();
    Serial.print("GPSManager::getYear() returning: ");
    Serial.println(result);
    return result;
}

int GPSManager::getMonth() {
    Serial.println("GPSManager::getMonth()");
    int result = useDefaults ? 8 : gps.date.month();
    Serial.print("GPSManager::getMonth() returning: ");
    Serial.println(result);
    return result;
}

int GPSManager::getDay() {
    Serial.println("GPSManager::getDay()");
    int result = useDefaults ? 27 : gps.date.day();
    Serial.print("GPSManager::getDay() returning: ");
    Serial.println(result);
    return result;
}

int GPSManager::getHour() {
    Serial.println("GPSManager::getHour()");
    int result = useDefaults ? 12 : gps.time.hour();
    Serial.print("GPSManager::getHour() returning: ");
    Serial.println(result);
    return result;
}

int GPSManager::getMinute() {
    Serial.println("GPSManager::getMinute()");
    int result = useDefaults ? 0 : gps.time.minute();
    Serial.print("GPSManager::getMinute() returning: ");
    Serial.println(result);
    return result;
}

int GPSManager::getSecond() {
    Serial.println("GPSManager::getSecond()");
    int result = useDefaults ? 0 : gps.time.second();
    Serial.print("GPSManager::getSecond() returning: ");
    Serial.println(result);
    return result;
}