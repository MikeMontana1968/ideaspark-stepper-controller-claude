#ifndef VOLTAGE_MONITOR_H
#define VOLTAGE_MONITOR_H

#include <Arduino.h>

class VoltageMonitor {
public:
    VoltageMonitor();
    ~VoltageMonitor();
    
    void begin();
    String getStatus();

private:
    float readInternalVoltage();
};

#endif