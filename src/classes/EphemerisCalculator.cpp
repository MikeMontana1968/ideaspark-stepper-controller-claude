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
    // Serial.print("EphemerisCalculator::getMoonSettingHeading(");
    // Serial.print(latitude);
    // Serial.print(", ");
    // Serial.print(longitude);
    // Serial.println(")");
    
    // Set observer location
    int latDeg = (int)latitude;
    int latMin = (int)((latitude - latDeg) * 60);
    float latSec = ((latitude - latDeg) * 60 - latMin) * 60;
    int lonDeg = (int)longitude;
    int lonMin = (int)((longitude - lonDeg) * 60);
    float lonSec = ((longitude - lonDeg) * 60 - lonMin) * 60;
    
    Ephemeris::setLocationOnEarth(latDeg, latMin, latSec, lonDeg, lonMin, lonSec);

    // Get current moon position
    SolarSystemObject moon = Ephemeris::solarSystemObjectAtDateAndTime(
        EarthsMoon, day(), month(), year(), hour(), minute(), second());
    
    // Convert equatorial coordinates to horizontal coordinates
    HorizontalCoordinates moonHorizontal = Ephemeris::equatorialToHorizontalCoordinatesAtDateAndTime(
        moon.equaCoordinates, day(), month(), year(), hour(), minute(), second());
    
    // Return azimuth as integer (compass heading for setting)
    int result = (int)(moonHorizontal.azi + 0.5);
    if (result < 0) result += 360;
    if (result >= 360) result -= 360;
    
    // Serial.print("EphemerisCalculator::getMoonSettingHeading() returning: ");
    // Serial.println(result);
    return result;
}

float EphemerisCalculator::getMoonAzimuth(float latitude, float longitude) {
    // Serial.print("EphemerisCalculator::getMoonAzimuth(");
    // Serial.print(latitude);
    // Serial.print(", ");
    // Serial.print(longitude);
    // Serial.println(")");
    
    // Set observer location
    int latDeg = (int)latitude;
    int latMin = (int)((latitude - latDeg) * 60);
    float latSec = ((latitude - latDeg) * 60 - latMin) * 60;
    int lonDeg = (int)longitude;
    int lonMin = (int)((longitude - lonDeg) * 60);
    float lonSec = ((longitude - lonDeg) * 60 - lonMin) * 60;
    
    Ephemeris::setLocationOnEarth(latDeg, latMin, latSec, lonDeg, lonMin, lonSec);
    
    SolarSystemObject moon = Ephemeris::solarSystemObjectAtDateAndTime(
        EarthsMoon, day(), month(), year(), hour(), minute(), second());
    HorizontalCoordinates moonHorizontal = Ephemeris::equatorialToHorizontalCoordinatesAtDateAndTime(
        moon.equaCoordinates, day(), month(), year(), hour(), minute(), second());
    
    float result = moonHorizontal.azi;
    // Serial.print("EphemerisCalculator::getMoonAzimuth() returning: ");
    // Serial.println(result);
    return result;
}

float EphemerisCalculator::getMoonElevation(float latitude, float longitude) {
    // Serial.print("EphemerisCalculator::getMoonElevation(");
    // Serial.print(latitude);
    // Serial.print(", ");
    // Serial.print(longitude);
    // Serial.println(")");
    
    // Set observer location
    int latDeg = (int)latitude;
    int latMin = (int)((latitude - latDeg) * 60);
    float latSec = ((latitude - latDeg) * 60 - latMin) * 60;
    int lonDeg = (int)longitude;
    int lonMin = (int)((longitude - lonDeg) * 60);
    float lonSec = ((longitude - lonDeg) * 60 - lonMin) * 60;
    
    Ephemeris::setLocationOnEarth(latDeg, latMin, latSec, lonDeg, lonMin, lonSec);
    
    SolarSystemObject moon = Ephemeris::solarSystemObjectAtDateAndTime(
        EarthsMoon, day(), month(), year(), hour(), minute(), second());
    HorizontalCoordinates moonHorizontal = Ephemeris::equatorialToHorizontalCoordinatesAtDateAndTime(
        moon.equaCoordinates, day(), month(), year(), hour(), minute(), second());
    
    float result = moonHorizontal.alt;
    // Serial.print("EphemerisCalculator::getMoonElevation() returning: ");
    // Serial.println(result);
    return result;
}

float EphemerisCalculator::calculateBearing(float lat1, float lon1, float lat2, float lon2) {
    // Serial.print("EphemerisCalculator::calculateBearing(");
    // Serial.print(lat1);
    // Serial.print(", ");
    // Serial.print(lon1);
    // Serial.print(", ");
    // Serial.print(lat2);
    // Serial.print(", ");
    // Serial.print(lon2);
    // Serial.println(")");
    
    float dLon = (lon2 - lon1) * PI / 180.0;
    lat1 = lat1 * PI / 180.0;
    lat2 = lat2 * PI / 180.0;
    
    float y = sin(dLon) * cos(lat2);
    float x = cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(dLon);
    
    float bearing = atan2(y, x) * 180.0 / PI;
    bearing = fmod((bearing + 360.0), 360.0);
    
    // Serial.print("EphemerisCalculator::calculateBearing() returning: ");
    // Serial.println(bearing);
    return bearing;
}

void EphemerisCalculator::getSunriseSunsetTimes(float latitude, float longitude, int& sunriseHour, int& sunriseMinute, int& sunsetHour, int& sunsetMinute) {
    // Set observer location
    int latDeg = (int)latitude;
    int latMin = (int)((latitude - latDeg) * 60);
    float latSec = ((latitude - latDeg) * 60 - latMin) * 60;
    int lonDeg = (int)longitude;
    int lonMin = (int)((longitude - lonDeg) * 60);
    float lonSec = ((longitude - lonDeg) * 60 - lonMin) * 60;
    
    Ephemeris::setLocationOnEarth(latDeg, latMin, latSec, lonDeg, lonMin, lonSec);
    Ephemeris::setAltitude(39);
    // Get sun rise and set times
    SolarSystemObject sun = Ephemeris::solarSystemObjectAtDateAndTime(
        Sun, day(), month(), year(), hour(), minute(), second());
    
    // Convert rise time from decimal hours to hours and minutes
    float sunriseFloat = sun.rise;
    sunriseHour = (int)sunriseFloat;
    sunriseMinute = (int)((sunriseFloat - sunriseHour) * 60);
    
    // Convert set time from decimal hours to hours and minutes  
    float sunsetFloat = sun.set;
    sunsetHour = (int)sunsetFloat;
    sunsetMinute = (int)((sunsetFloat - sunsetHour) * 60);
}

