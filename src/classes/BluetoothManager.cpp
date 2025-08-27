#include "BluetoothManager.h"
#include "LogManager.h"
#include "ConfigurationManager.h"

extern LogManager* logManager;
extern ConfigurationManager* configManager;

BluetoothManager::BluetoothManager() {
    Serial.println("BluetoothManager::BluetoothManager()");
    userInteracting = false;
    inputBuffer = "";
    menuState = 0;
}

BluetoothManager::~BluetoothManager() {
    Serial.println("BluetoothManager::~BluetoothManager()");
}

void BluetoothManager::begin() {
    Serial.println("BluetoothManager::begin()");
    btSerial.begin("ESP32_StepperController");
    btSerial.println("Bluetooth initialized");
}

void BluetoothManager::handleUserInteraction() {
    // Serial.println("BluetoothManager::handleUserInteraction()"); // Commented out - called frequently
    
    if (btSerial.available()) {
        char c = btSerial.read();
        
        if (c == '\r' || c == '\n') {
            if (!userInteracting && inputBuffer.length() == 0) {
                // User pressed Enter to initiate interaction
                userInteracting = true;
                sendLastLogLines();
                showMainMenu();
            } else if (userInteracting && inputBuffer.length() > 0) {
                // Process the input
                if (menuState == 0) {
                    processMenuSelection(inputBuffer.charAt(0));
                }
                inputBuffer = "";
            }
        } else if (c >= 32 && c <= 126) { // Printable characters
            inputBuffer += c;
        }
    }
}

void BluetoothManager::showMainMenu() {
    Serial.println("BluetoothManager::showMainMenu()");
    btSerial.println("1. Rotate the stepper 1 rotation per minute");
    btSerial.println("2. Rotate the stepper 1 rotation per hour");
    btSerial.println("3. Rotate the stepper 1 rotation per day");
    btSerial.println("4. Custom configuration");
    btSerial.println("5. Display last log");
    btSerial.println("6. Clear logs");
    btSerial.print("Select option: ");
}

void BluetoothManager::processMenuSelection(char selection) {
    Serial.print("BluetoothManager::processMenuSelection(");
    Serial.print(selection);
    Serial.println(")");
    
    Configuration config;
    
    switch (selection) {
        case '1':
            config.rotationSpeed = ONCE_PER_MINUTE;
            config.startTime = "00:00";
            config.durationHours = 0;
            config.rewindAfterComplete = false;
            configManager->setConfiguration(config);
            btSerial.println("Configuration set: 1 rotation per minute");
            logManager->logInfo("Configuration changed to 1 rotation per minute");
            resetMenuState();
            break;
            
        case '2':
            config.rotationSpeed = ONCE_PER_HOUR;
            config.startTime = "00:00";
            config.durationHours = 0;
            config.rewindAfterComplete = false;
            configManager->setConfiguration(config);
            btSerial.println("Configuration set: 1 rotation per hour");
            logManager->logInfo("Configuration changed to 1 rotation per hour");
            resetMenuState();
            break;
            
        case '3':
            config.rotationSpeed = ONCE_PER_DAY;
            config.startTime = "00:00";
            config.durationHours = 0;
            config.rewindAfterComplete = false;
            configManager->setConfiguration(config);
            btSerial.println("Configuration set: 1 rotation per day");
            logManager->logInfo("Configuration changed to 1 rotation per day");
            resetMenuState();
            break;
            
        case '4':
            handleCustomConfiguration();
            break;
            
        case '5':
            displayLastLog();
            resetMenuState();
            break;
            
        case '6':
            clearLogs();
            resetMenuState();
            break;
            
        default:
            btSerial.println("Invalid selection. Try again.");
            showMainMenu();
            break;
    }
}

void BluetoothManager::handleCustomConfiguration() {
    Serial.println("BluetoothManager::handleCustomConfiguration()");
    // This is a simplified version - in practice, you'd need state management
    // for multi-step input collection
    btSerial.println("Custom configuration mode not fully implemented in this version");
    btSerial.println("Please use options 1-3 for now");
    resetMenuState();
}

void BluetoothManager::displayLastLog() {
    Serial.println("BluetoothManager::displayLastLog()");
    if (logManager) {
        String logContent = logManager->getLastLogContent();
        btSerial.println("=== Current Log Content ===");
        btSerial.println(logContent);
        btSerial.println("=== End Log ===");
    } else {
        btSerial.println("Log manager not available");
    }
}

void BluetoothManager::clearLogs() {
    Serial.println("BluetoothManager::clearLogs()");
    if (logManager) {
        logManager->clearAllLogs();
        btSerial.println("All logs cleared");
        logManager->logInfo("All logs cleared by user");
    } else {
        btSerial.println("Log manager not available");
    }
}

void BluetoothManager::sendLastLogLines() {
    Serial.println("BluetoothManager::sendLastLogLines()");
    if (logManager) {
        String lastLines = logManager->getLastLogLines(5);
        btSerial.println("=== Last 5 Log Entries ===");
        btSerial.println(lastLines);
        btSerial.println();
    } else {
        btSerial.println("Log manager not available");
    }
}

void BluetoothManager::resetMenuState() {
    Serial.println("BluetoothManager::resetMenuState()");
    userInteracting = false;
    inputBuffer = "";
    menuState = 0;
}

void BluetoothManager::sendPrompt(const String& prompt) {
    Serial.print("BluetoothManager::sendPrompt(");
    Serial.print(prompt);
    Serial.println(")");
    btSerial.print(prompt);
}

String BluetoothManager::readInput() {
    Serial.println("BluetoothManager::readInput()");
    String result = inputBuffer;
    inputBuffer = "";
    Serial.print("BluetoothManager::readInput() returning: ");
    Serial.println(result);
    return result;
}