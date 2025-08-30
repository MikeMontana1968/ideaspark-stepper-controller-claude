#include "Ephemeris.h"

Ephemeris::Ephemeris(GPSManager* gpsManager) : gpsManager(gpsManager), currentTime(0), latitude(0.0), longitude(0.0) {
    if (gpsManager) {
        setCurrentTime(gpsManager->getUnixTimestamp());
        setLatitude(gpsManager->getLatitude());
        setLongitude(gpsManager->getLongitude());
    }
    
}

Ephemeris::~Ephemeris() {
}

void Ephemeris::setCurrentTime(unsigned long unixTimestamp) {
    currentTime = unixTimestamp;
}

unsigned long Ephemeris::getCurrentTime() {
    return currentTime;
}

void Ephemeris::setLatitude(double lat) {
    latitude = lat;
}

double Ephemeris::getLatitude() {
    return latitude;
}

void Ephemeris::setLongitude(double lng) {
    longitude = lng;
}

double Ephemeris::getLongitude() {
    return longitude;
}

String Ephemeris::printDate(time_t date) {
  char buff[20];
  sprintf(buff, "%2d-%02d-%4d %02d:%02d:%02d", day(date), month(date), year(date), hour(date), minute(date), second(date));
  return String(buff);
}

String Ephemeris::hhmm(time_t date) {
  char buff[20];
  sprintf(buff, "%02d:%02d", hour(date), minute(date));
  return String(buff);
}

String Ephemeris::getDayName(time_t date) {
  const char* dayNames[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
  return String(dayNames[weekday(date) - 1]);
}

String Ephemeris::printTime(double hours) {
  int h = (int)hours;
  int m = (int)((hours - h) * 60);
  int s = (int)((hours - h - m/60.0) * 3600);
  String result = "";
  if (h < 10) result += "0";
  result += String(h);
  result += ":";
  if (m < 10) result += "0";
  result += String(m);
  result += ":";
  if (s < 10) result += "0";
  result += String(s);
  return result;
}

String Ephemeris::getAlmanacSummary() {
  this->moonCalc.calculate(this->getLatitude(), this->getLongitude(), now());

  int timezoneOffset = gpsManager->getTimezoneOffset();
  time_t utc = now() - (timezoneOffset * 3600); // Convert local time back to UTC for SolarCalculator
  
  double transit, sunrise, sunset, sunSetAz, sunElevation;
  calcSunriseSunset(utc, this->getLatitude(), this->getLongitude(), transit, sunrise, sunset);

  calcHorizontalCoordinates(utc, latitude, longitude, sunSetAz, sunElevation);
  time_t dSunRise = doubleToTimeT(sunrise + timezoneOffset);
  time_t dSunSet = doubleToTimeT(sunset + timezoneOffset);
  String result = "S: " + hhmm(dSunRise) + " " + hhmm(dSunSet)
    + " " + String(int(sunSetAz)) + "'"
    + "\n";
 
  moonCalc.calculate(this->getLatitude(), this->getLongitude(), now());
  
  // Moon status logic
  time_t currentTime = now();
  const long TWELVE_HOURS = 12 * 3600; // 12 hours in seconds
  
  if (!moonCalc.hasRise) {
    result += "No Moon Today\n";
  } else {
    // Check if moon rose less than 12 hours ago
    // long timeSinceRise = currentTime - moonCalc.riseTime;
    // long timeUntilRise = moonCalc.riseTime - currentTime;
        
    result += "Moon ^ " + getDayName(moonCalc.riseTime) + " " + hhmm(moonCalc.riseTime) + " " + String(int(moonCalc.riseAz)) + "'\n";
    result += "Moon v " + getDayName(moonCalc.setTime)  + " " + hhmm(moonCalc.setTime)  + " " + String(int(moonCalc.setAz))  + "'\n";
  }
  
  result += String(int(getMoonPhase(now()) * 100)) + "% Full";
  
result += "\n";
  return result;
}

time_t Ephemeris::doubleToTimeT(double hours) {
  int h = (int)hours;
  int m = (int)((hours - h) * 60);
  int s = (int)((hours - h - m/60.0) * 3600);
  
  time_t currentTime = now();
  tmElements_t tm;
  breakTime(currentTime, tm);
  
  tm.Hour = h;
  tm.Minute = m;
  tm.Second = s;
  
  return makeTime(tm);
}

double Ephemeris::getMoonPhase(time_t t) {
  // Calculate Julian date from Unix timestamp
  double jd = (double)t / 86400.0 + 2440587.5;
  
  // Calculate days since J2000.0
  double daysSinceJ2000 = jd - 2451545.0;
  
  // Mean elongation of Moon from Sun (D)
  double D = 297.8501921 + 445267.1114034 * daysSinceJ2000 / 36525.0;
  
  // Sun's mean anomaly (M)
  double M = 357.5291092 + 35999.0502909 * daysSinceJ2000 / 36525.0;
  
  // Moon's mean anomaly (M')
  double Mp = 134.9633964 + 477198.8675055 * daysSinceJ2000 / 36525.0;
  
  // Convert to radians
  D = D * PI / 180.0;
  M = M * PI / 180.0;
  Mp = Mp * PI / 180.0;
  
  // Phase angle calculation (simplified)
  double phase = 0.5 * (1.0 - cos(Mp));
  
  return phase; // Returns 0.0 (new moon) to 1.0 (full moon)
}

