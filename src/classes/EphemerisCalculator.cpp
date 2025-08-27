#include "EphemerisCalculator.h"
#include <TimeLib.h>
#include <math.h>

EphemerisCalculator::EphemerisCalculator() {
    Serial.println("EphemerisCalculator::EphemerisCalculator()");
}

EphemerisCalculator::~EphemerisCalculator() {
    Serial.println("EphemerisCalculator::~EphemerisCalculator()");
}

int EphemerisCalculator::getMoonSettingHeading(float latitude, float longitude) {
    Serial.print("EphemerisCalculator::getMoonSettingHeading(");
    Serial.print(latitude);
    Serial.print(", ");
    Serial.print(longitude);
    Serial.println(")");
    
    float jd;
    getCurrentJulianDate(jd);
    Serial.println("EphemerisCalculator::getMoonSettingHeading() Not Implemented");
    return 1.0;

    // Get current moon position
    // SolarSystemObject moon = ephemeris.moonPosition(jd);
    
    // // Convert equatorial coordinates to horizontal coordinates
    // HorizontalCoordinates moonHorizontal = ephemeris.equatorialToHorizontal(
    //     moon.equatorialCoordinates, jd, latitude, longitude);
    
    // // Return azimuth as integer (compass heading for setting)
    // int result = (int)(moonHorizontal.azimuth + 0.5);
    // if (result < 0) result += 360;
    // if (result >= 360) result -= 360;
    
    // Serial.print("EphemerisCalculator::getMoonSettingHeading() returning: ");
    // Serial.println(result);
    // return result;
}

float EphemerisCalculator::getMoonAzimuth(float latitude, float longitude) {
    Serial.print("EphemerisCalculator::getMoonAzimuth(");
    Serial.print(latitude);
    Serial.print(", ");
    Serial.print(longitude);
    Serial.println(")");
    
    float jd;
    getCurrentJulianDate(jd);
    Serial.println("EphemerisCalculator::getMoonAzimuth() Not Implemented");
    return 1.0;
    // SolarSystemObject moon = ephemeris.moonPosition(jd);
    // HorizontalCoordinates moonHorizontal = ephemeris.equatorialToHorizontal(
    //     moon.equatorialCoordinates, jd, latitude, longitude);
    
    // float result = moonHorizontal.azimuth;
    // Serial.print("EphemerisCalculator::getMoonAzimuth() returning: ");
    // Serial.println(result);
    // return result;
}

float EphemerisCalculator::getMoonElevation(float latitude, float longitude) {
    Serial.print("EphemerisCalculator::getMoonElevation(");
    Serial.print(latitude);
    Serial.print(", ");
    Serial.print(longitude);
    Serial.println(")");
    
    float jd;
    getCurrentJulianDate(jd);
    Serial.println("EphemerisCalculator::getMoonElevation() Not Implemented");
    return 1.0;
    // SolarSystemObject moon = ephemeris.moonPosition(jd);
    // HorizontalCoordinates moonHorizontal = ephemeris.equatorialToHorizontal(
    //     moon.equatorialCoordinates, jd, latitude, longitude);
    
    // float result = moonHorizontal.elevation;
    Serial.print("EphemerisCalculator::getMoonElevation() returning: ");
    //Serial.println(result);
    //return result;
}

float EphemerisCalculator::calculateBearing(float lat1, float lon1, float lat2, float lon2) {
    Serial.print("EphemerisCalculator::calculateBearing(");
    Serial.print(lat1);
    Serial.print(", ");
    Serial.print(lon1);
    Serial.print(", ");
    Serial.print(lat2);
    Serial.print(", ");
    Serial.print(lon2);
    Serial.println(")");
    
    float dLon = (lon2 - lon1) * PI / 180.0;
    lat1 = lat1 * PI / 180.0;
    lat2 = lat2 * PI / 180.0;
    
    float y = sin(dLon) * cos(lat2);
    float x = cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(dLon);
    
    float bearing = atan2(y, x) * 180.0 / PI;
    bearing = fmod((bearing + 360.0), 360.0);
    
    Serial.print("EphemerisCalculator::calculateBearing() returning: ");
    Serial.println(bearing);
    return bearing;
}

void EphemerisCalculator::getCurrentJulianDate(float& jd) {
    Serial.println("EphemerisCalculator::getCurrentJulianDate()");
    
    // Convert current time to Julian Date
    int y = year();
    int m = month();
    int d = day();
    int h = hour();
    int min = minute();
    int s = second();
    
    // Julian Date calculation
    if (m <= 2) {
        y -= 1;
        m += 12;
    }
    
    int a = y / 100;
    int b = 2 - a + (a / 4);
    
    jd = (int)(365.25 * (y + 4716)) + (int)(30.6001 * (m + 1)) + d + b - 1524.5;
    jd += (h + min / 60.0 + s / 3600.0) / 24.0;
    
    Serial.print("EphemerisCalculator::getCurrentJulianDate() calculated: ");
    Serial.println(jd);
}