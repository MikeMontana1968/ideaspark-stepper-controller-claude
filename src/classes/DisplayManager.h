#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

class DisplayManager {
public:
    DisplayManager();
    ~DisplayManager();
    
    void begin();
    void updateDisplay(const String& statusText, const String& activityText);
    void clearDisplay();

private:
    Adafruit_SSD1306* display;
    String lastStatusText;
    String lastActivityText;
};

#endif