#ifndef EPHEMERIS_H
#define EPHEMERIS_H

#include <Arduino.h>
#include "GPSManager.h"
#include <SolarCalculator.h> // For sun data
#include <MoonRise.h> // For moon data
#include <TimeLib.h>
class Ephemeris {
public:
    Ephemeris(GPSManager* gpsManager);
    ~Ephemeris();
    
    void setCurrentTime(unsigned long unixTimestamp);
    unsigned long getCurrentTime();
    
    void setLatitude(double latitude);
    double getLatitude();
    
    void setLongitude(double longitude);
    double getLongitude();

    String printDate(time_t date);

    String hhmm(time_t date);
    
    String getDayName(time_t date);

    String printTime(double hours);

    String getAlmanacSummary();

    time_t doubleToTimeT(double hours);

    double getMoonPhase(time_t t);

private:
    GPSManager* gpsManager;
    unsigned long currentTime;
    double latitude;
    double longitude;
    MoonRise moonCalc;
};

#endif