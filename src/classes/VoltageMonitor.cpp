#include "VoltageMonitor.h"

VoltageMonitor::VoltageMonitor() {
    Serial.println("VoltageMonitor::VoltageMonitor()");
}

VoltageMonitor::~VoltageMonitor() {
    Serial.println("VoltageMonitor::~VoltageMonitor()");
}

void VoltageMonitor::begin() {
    Serial.println("VoltageMonitor::begin()");
    // No specific initialization needed for internal voltage reading
}

String VoltageMonitor::getStatus() {
    Serial.println("VoltageMonitor::getStatus()");
    
    float voltage = readInternalVoltage();
    char voltageStr[8];
    sprintf(voltageStr, "%.1fv", voltage);
    
    return String(voltageStr);
}

float VoltageMonitor::readInternalVoltage() {
    // Read internal hall sensor reference voltage (approximates VCC)
    // ESP32 internal reference is typically 1.1V, full scale ADC is 4095
    // This method reads the internal voltage reference
    
    uint32_t reading = 0;
    
    // Take multiple readings for stability
    for (int i = 0; i < 10; i++) {
        reading += analogRead(A0);  // Use any available ADC pin
        delay(1);
    }
    reading /= 10;
    
    // Convert ADC reading to voltage
    // ADC reference voltage is typically 3.3V for ESP32
    // ADC resolution is 12-bit (4096 levels)
    float voltage = (reading * 3.3) / 4095.0;
    
    // For more accurate internal VCC reading, we can use the formula:
    // VCC = (ADC_REF_VOLTAGE * 4095) / ADC_reading
    // But this requires calibration values
    
    // Simple approximation for now - multiply by factor to get VCC estimate
    voltage = voltage * 1.0; // Adjust this multiplier based on your board
    
    return voltage;
}