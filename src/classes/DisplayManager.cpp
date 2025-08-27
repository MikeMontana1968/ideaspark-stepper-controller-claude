#include "DisplayManager.h"

DisplayManager::DisplayManager() {
    Serial.println("DisplayManager::DisplayManager()");
    display = nullptr;
}

DisplayManager::~DisplayManager() {
    Serial.println("DisplayManager::~DisplayManager()");
    if (display) {
        delete display;
    }
}

void DisplayManager::begin() {
    Serial.println("DisplayManager::begin()");
    display = new Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
    
    if (!display->begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("SSD1306 allocation failed");
        return;
    }
    
    display->clearDisplay();
    display->setTextSize(1);
    display->setTextColor(WHITE);
    display->setCursor(0, 0);
    display->println("Initializing...");
    display->display();
}

void DisplayManager::updateDisplay(const String& statusText, const String& activityText) {
    Serial.print("DisplayManager::updateDisplay(");
    Serial.print(statusText);
    Serial.print(", ");
    Serial.print(activityText);
    Serial.println(")");
    
    // Only update if text has changed
    if (statusText == lastStatusText && activityText == lastActivityText) {
        return;
    }
    
    lastStatusText = statusText;
    lastActivityText = activityText;
    
    if (!display) {
        return;
    }
    
    display->clearDisplay();
    
    // Status bar (line 1) - 9 point sans serif (size 1)
    display->setTextSize(1);
    display->setCursor(0, 0);
    display->println(statusText);
    
    // Activity area (starting from line 2)
    display->setCursor(0, 16);
    display->println(activityText);
    
    display->display();
}

void DisplayManager::clearDisplay() {
    Serial.println("DisplayManager::clearDisplay()");
    if (display) {
        display->clearDisplay();
        display->display();
    }
}