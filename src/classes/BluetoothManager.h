#ifndef BLUETOOTH_MANAGER_H
#define BLUETOOTH_MANAGER_H

#include <Arduino.h>
#include <BluetoothSerial.h>

class BluetoothManager {
public:
    BluetoothSerial btSerial;
    bool userInteracting;
    String inputBuffer;
    int menuState;
    bool isConnected;
    
    BluetoothManager();
    ~BluetoothManager();
    
    void begin();
    void handleUserInteraction();
    void showMainMenu();
    void processMenuSelection(char selection);
    void handleCustomConfiguration();
    void displayLastLog();
    void clearLogs();
    void sendLastLogLines();
    

private:
    void resetMenuState();
    void sendPrompt(const String& prompt);
    String readInput();
};

#endif