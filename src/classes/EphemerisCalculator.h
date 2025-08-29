#ifndef EPHEMERIS_CALCULATOR_H
#define EPHEMERIS_CALCULATOR_H

#include <Arduino.h>
#include <Ephemeris.h>

class EphemerisCalculator {
public:
    EphemerisCalculator();
    ~EphemerisCalculator();
    
    int getMoonSettingHeading(float latitude, float longitude);
    float getMoonAzimuth(float latitude, float longitude);
    float getMoonElevation(float latitude, float longitude);
    void getSunriseSunsetTimes(float latitude, float longitude, int& sunriseHour, int& sunriseMinute, int& sunsetHour, int& sunsetMinute);

private:
    float calculateBearing(float lat1, float lon1, float lat2, float lon2);
};

#endif