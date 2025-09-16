# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

ESP32-based GPS-enabled stepper motor controller with OLED display, Bluetooth configuration, and astronomical calculations. The system tracks time via GPS, rotates a stepper motor at configurable rates (per minute/hour/day), and calculates moon setting headings using ephemeris data.

## Build and Development Commands

```bash
# Build the project
pio run

# Upload to ESP32
pio run --target upload

# Monitor serial output
pio device monitor --baud 115200

# Clean build
pio run --target clean

# Check for dependency updates
pio pkg update
```

## Architecture Overview

The system uses a component-based architecture with dedicated manager classes:

### Core Components

- **main.cpp**: Application entry point, orchestrates all managers, handles main loop timing
- **GPSManager**: Handles NEO-6M GPS module communication, time synchronization, and fallback to default location after 10-minute timeout
- **StepperController**: Controls 28BYJ-48 stepper motor via ULN2003 driver using AccelStepper library
- **DisplayManager**: Manages built-in OLED (SSD1306) display updates every 250ms
- **BluetoothManager**: Provides user configuration interface via Bluetooth Serial Profile
- **ConfigurationManager**: Handles JSON-based configuration persistence in SPIFFS at `/schedule.json`
- **LogManager**: SPIFFS-based logging system with automatic rotation (1MB files, max 50 files, numbered from 1000)
- **Ephemeris**: Calculates moon setting compass headings using SolarCalculator and MoonRise libraries

### Data Flow

1. GPS data updates continuously, system time set once GPS fix obtained
2. Configuration loaded from SPIFFS on startup, modified via Bluetooth interface  
3. Stepper position calculated based on rotation speed and elapsed time
4. Display updated every 250ms with time, position, coordinates, and moon heading
5. Logs written every minute with complete system status

### Hardware Configuration

- **GPIO Conflict Note**: Current wiring has conflicts between I2C (GPIO21/22) and stepper pins. See `docs/wiring-pin-out.md` for recommended GPIO revision
- **Default Location**: Falls back to 40.5169°N, 74.4063°W if GPS timeout occurs
- **Power Requirements**: 5V supply needed for stepper motor operation

### Configuration Structure

Settings stored as JSON in `/schedule.json`:
```json
{
  "rotationSpeed": 2,        // 0=per minute, 1=per hour, 2=per day  
  "startTime": "00:00",      // HH:MM format
  "durationHours": 0,        // 0 = indefinite rotation
  "rewindAfterComplete": false
}
```

### Key Timing Constants

- Display update interval: 250ms
- Serial output interval: 5 seconds  
- Log entry interval: 1 minute
- GPS timeout: 10 minutes
- Log file rotation: 1MB per file

## Dependencies

Key libraries managed by PlatformIO:
- **mikalhart/TinyGPSPlus@^1.0.3** - GPS data parsing
- **adafruit/Adafruit GFX Library@^1.11.9** - Graphics primitives
- **adafruit/Adafruit SSD1306@^2.5.10** - OLED display driver
- **waspinator/AccelStepper@^1.64** - Stepper motor control
- **bblanchon/ArduinoJson@^7.4.2** - JSON configuration handling
- **paulstoffregen/Time@^1.6.1** - Time management utilities
- **jpb10/SolarCalculator@^2.0.2** - Solar position calculations
- **signetica/MoonRise@^2.0.4** - Moon position calculations
- **plerup/EspSoftwareSerial@^8.2.0** - Software serial for GPS

## GPIO Pin Assignments

**Current wiring (with conflicts):**
- GPS: GPIO16 (RX), GPIO17 (TX)
- Stepper: GPIO18, GPIO19, GPIO21, GPIO22
- I2C OLED: GPIO21 (SDA), GPIO22 (SCL)

**Note**: GPIO21/22 have conflicts between I2C and stepper. See `docs/wiring-pin-out.md` for recommended revision using GPIO12, GPIO14, GPIO26, GPIO27 for stepper control.

## Testing and Validation

No automated test framework is configured. Testing requires hardware-in-the-loop validation:

1. Upload code to ESP32
2. Monitor serial output at 115200 baud
3. Verify GPS acquisition within 10 minutes
4. Test Bluetooth configuration via terminal connection
5. Validate stepper motor operation and display updates