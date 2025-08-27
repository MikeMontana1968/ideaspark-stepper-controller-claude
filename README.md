# ESP32 Stepper Motor Controller

A GPS-enabled stepper motor controller with OLED display and Bluetooth configuration interface.

## Hardware Requirements

- Ideaspark ESP32 0.96 LED development board
- NEO-6M GPS Module
- ULN2003 Stepper Driver Board
- 28BYJ-48 Stepper Motor
- Jumper wires for connections

## Features

- **GPS Time Synchronization**: Automatically sets system time from GPS
- **Multiple Rotation Speeds**: 1 rotation per minute, hour, or day
- **OLED Status Display**: Shows time, position, coordinates, and moon heading
- **Bluetooth Configuration**: Configure settings via Bluetooth terminal
- **Comprehensive Logging**: SPIFFS-based logging with automatic rotation
- **Moon Tracking**: Calculate moon setting compass heading using ephemeris

## Software Dependencies

All dependencies are managed by PlatformIO:

- `mikalhart/TinyGPSPlus@^1.0.3`
- `adafruit/Adafruit GFX Library@^1.11.9`
- `adafruit/Adafruit SSD1306@^2.5.10`
- `waspinator/AccelStepper@^1.64`
- `bblanchon/ArduinoJson@^7.4.2`
- `marscaper/Ephemeris@^1.0.1`
- `paulstoffregen/Time@^1.6.1`

## Installation

1. Clone this repository
2. Open in PlatformIO IDE or VS Code with PlatformIO extension
3. Connect hardware according to pin-out diagram
4. Build and upload to ESP32

## Usage

### Initial Setup
1. Power on the device
2. Wait for GPS fix (up to 10 minutes) or it will use default location
3. Connect to Bluetooth device "ESP32_StepperController"

### Bluetooth Configuration
1. Press Enter in Bluetooth terminal to initiate configuration
2. Select from menu options:
   - Options 1-3: Simple rotation speeds
   - Option 4: Custom configuration (future enhancement)
   - Option 5: Display current log
   - Option 6: Clear all logs

### Display Information
- **Line 1**: Time, motor degrees, latitude, longitude
- **Line 2**: Mode, moon heading, remaining time or "completed"

## File Structure

```
src/
├── main.cpp                    # Main application entry point
└── classes/
    ├── GPSManager.h/.cpp       # GPS handling and time sync
    ├── StepperController.h/.cpp # Motor control and timing
    ├── DisplayManager.h/.cpp   # OLED display management
    ├── BluetoothManager.h/.cpp # BT configuration interface
    ├── ConfigurationManager.h/.cpp # Settings persistence
    ├── LogManager.h/.cpp       # SPIFFS logging system
    └── EphemerisCalculator.h/.cpp # Moon position calculations
```

## Configuration

Settings are stored in `/schedule.json` on SPIFFS with the following structure:

```json
{
  "rotationSpeed": 2,
  "startTime": "00:00",
  "durationHours": 0,
  "rewindAfterComplete": false
}
```

## Logging

- Logs stored in `/logs/` folder on SPIFFS
- Each log file is max 1MB, numbered sequentially starting from 1000
- Maximum 50 log files retained
- Automatic rotation and cleanup

## Default Location

If GPS fix is not obtained within 10 minutes:
- **Latitude**: 40.5169° N
- **Longitude**: 74.4063° W  
- **Elevation**: 0m
- **Time**: C++ build time

## Version

Prompt Document Version 1.0.2

## License

This project is provided as-is for educational and development purposes.