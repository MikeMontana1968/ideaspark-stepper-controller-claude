# ESP32 Stepper Motor Controller - Pin-out Diagram

## Hardware Connections

### ESP32 0.96 LED Dev Board
- **Power**: 5V/GND from USB or external power supply
- **Built-in OLED**: Uses I2C (SDA: GPIO21, SCL: GPIO22) - Address 0x3C
- **Built-in LED**: GPIO2

### NEO-6M GPS Module Connections
```
GPS Module    →    ESP32 Pin
VCC           →    3.3V
GND           →    GND
TX            →    GPIO16 (RX)
RX            →    GPIO17 (TX)
```

### ULN2003 Stepper Driver Board Connections
```
ULN2003       →    ESP32 Pin
VCC           →    5V
GND           →    GND
IN1           →    GPIO18
IN2           →    GPIO19
IN3           →    GPIO21
IN4           →    GPIO22
```

### 28BYJ-48 Stepper Motor Connections
```
Stepper Motor →    ULN2003 Driver
5-pin connector →   Motor connector on ULN2003 board
```

## Complete Pin Assignment Summary

| Component | Function | ESP32 Pin | Notes |
|-----------|----------|-----------|-------|
| **Built-in OLED** | I2C SDA | GPIO21 | Default I2C pins |
| **Built-in OLED** | I2C SCL | GPIO22 | Default I2C pins |
| **GPS Module** | UART RX | GPIO16 | GPS TX connects here |
| **GPS Module** | UART TX | GPIO17 | GPS RX connects here |
| **Stepper Driver** | Control IN1 | GPIO18 | ULN2003 input 1 |
| **Stepper Driver** | Control IN2 | GPIO19 | ULN2003 input 2 |
| **Stepper Driver** | Control IN3 | GPIO21 | ULN2003 input 3 |
| **Stepper Driver** | Control IN4 | GPIO22 | ULN2003 input 4 |
| **Built-in LED** | Status LED | GPIO2 | On-board LED |
| **Bluetooth** | Built-in | N/A | ESP32 integrated BLE |

## Power Requirements
- **ESP32**: 3.3V/5V via USB or VIN pin
- **GPS Module**: 3.3V (80mA typical)
- **ULN2003 + Stepper**: 5V (200-300mA when stepping)
- **Total Power**: ~500mA at 5V recommended

## Fritzing Diagram Description
```
┌─────────────────────────────────────────┐
│           ESP32 0.96 LED Board          │
│  ┌─────────────────────────────────┐    │
│  │         Built-in OLED           │    │
│  │        (I2C: 21,22)             │    │
│  └─────────────────────────────────┘    │
│                                         │
│  GPIO16 ────────────────────────────────┼──→ GPS TX
│  GPIO17 ────────────────────────────────┼──→ GPS RX  
│  3.3V ──────────────────────────────────┼──→ GPS VCC
│  GND ───────────────────────────────────┼──→ GPS GND
│                                         │
│  GPIO18 ────────────────────────────────┼──→ ULN2003 IN1
│  GPIO19 ────────────────────────────────┼──→ ULN2003 IN2
│  GPIO21 ────────────────────────────────┼──→ ULN2003 IN3
│  GPIO22 ────────────────────────────────┼──→ ULN2003 IN4
│  5V ────────────────────────────────────┼──→ ULN2003 VCC
│  GND ───────────────────────────────────┼──→ ULN2003 GND
│                                         │
└─────────────────────────────────────────┘

┌─────────────────┐    ┌──────────────────┐
│   NEO-6M GPS    │    │   ULN2003 Driver │
│                 │    │                  │
│  VCC  GND       │    │  IN1 IN2 IN3 IN4 │
│   TX   RX       │    │                  │
└─────────────────┘    │  Motor Connector │
                       │        ↓         │
                       │  ┌─────────────┐ │
                       │  │ 28BYJ-48    │ │
                       │  │ Stepper     │ │
                       │  │ Motor       │ │
                       │  └─────────────┘ │
                       └──────────────────┘
```

## Wiring Notes
1. **Double-check GPIO assignments** - The stepper uses GPIO18-22, but the OLED also uses GPIO21-22 for I2C
2. **Power considerations** - Use adequate power supply for stepper motor operation
3. **Ground connections** - Ensure all components share common ground
4. **GPIO conflicts** - Note that GPIO21/22 are shared between I2C and stepper - this may need revision
5. **Bluetooth** - No external wiring needed, uses ESP32's built-in Bluetooth

## Recommended GPIO Revision
For better pin allocation without conflicts:
```
Component         →    Revised ESP32 Pin
Stepper IN1      →    GPIO12
Stepper IN2      →    GPIO14
Stepper IN3      →    GPIO27
Stepper IN4      →    GPIO26
I2C SDA (OLED)   →    GPIO21 (keep)
I2C SCL (OLED)   →    GPIO22 (keep)
GPS RX           →    GPIO16 (keep)
GPS TX           →    GPIO17 (keep)
```