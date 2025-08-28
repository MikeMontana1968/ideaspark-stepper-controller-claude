#include "LogManager.h"
#include <TimeLib.h>

LogManager::LogManager() {
    Serial.println("LogManager::LogManager()");
    currentLogNumber = startingLogNumber;
    currentLogSize = 0;
}

LogManager::~LogManager() {
    Serial.println("LogManager::~LogManager()");
    if (currentLogFile) {
        currentLogFile.close();
    }
}

void LogManager::begin() {
    Serial.println("LogManager::begin()");
    
    createLogsFolder();
    
    // Find the highest existing log number
    File root = SPIFFS.open("/logs");
    if (root && root.isDirectory()) {
        File file = root.openNextFile();
        while (file) {
            String fileName = file.name();
            if (fileName.endsWith(".log")) {
                int logNum = fileName.substring(fileName.lastIndexOf('/') + 1).toInt();
                if (logNum >= currentLogNumber) {
                    currentLogNumber = logNum + 1;
                }
            }
            file = root.openNextFile();
        }
        root.close();
    }
    
    openNewLogFile();
    logInfo("Log system initialized");
}

void LogManager::logInfo(const String& message) {
    // Serial.print("LogManager::logInfo(");
    // Serial.print(message);
    // Serial.println(")");
    writeLogEntry("INFO", message);
}

void LogManager::logError(const String& message) {
    // Serial.print("LogManager::logError(");
    // Serial.print(message);
    // Serial.println(")");
    writeLogEntry("ERROR", message);
}

String LogManager::getLastLogContent() {
    Serial.println("LogManager::getLastLogContent()");
    
    if (currentLogFile) {
        currentLogFile.close();
    }
    
    String fileName = getCurrentLogFileName();
    File file = SPIFFS.open(fileName, "r");
    if (!file) {
        Serial.print("LogManager::getLastLogContent() returning: ");
        Serial.println("No log file available");
        return "No log file available";
    }
    
    String content = file.readString();
    file.close();
    
    // Reopen current log file for writing
    openNewLogFile();
    
    // Serial.print("LogManager::getLastLogContent() returning content of length: ");
    // Serial.println(content.length());
    return content;
}

String LogManager::getLastLogLines(int numLines) {
    // Serial.print("LogManager::getLastLogLines(");
    // Serial.print(numLines);
    // Serial.println(")");
    
    String content = getLastLogContent();
    
    // Split into lines and get the last numLines
    int lineCount = 0;
    int lastNewline = content.length();
    
    for (int i = content.length() - 1; i >= 0 && lineCount < numLines; i--) {
        if (content.charAt(i) == '\n') {
            lineCount++;
            if (lineCount == numLines) {
                String result = content.substring(i + 1);
                // Serial.print("LogManager::getLastLogLines() returning: ");
                // Serial.println(result);
                return result;
            }
        }
    }
    
    // Serial.print("LogManager::getLastLogLines() returning: ");
    // Serial.println(content);
    return content;
}

void LogManager::clearAllLogs() {
    Serial.println("LogManager::clearAllLogs()");
    
    if (currentLogFile) {
        currentLogFile.close();
    }
    
    // Remove all log files
    File root = SPIFFS.open("/logs");
    if (root && root.isDirectory()) {
        File file = root.openNextFile();
        while (file) {
            String filePath = "/logs/" + String(file.name());
            file.close();
            SPIFFS.remove(filePath);
            file = root.openNextFile();
        }
        root.close();
    }
    
    // Reset log number and create new log file
    currentLogNumber = startingLogNumber;
    currentLogSize = 0;
    openNewLogFile();
    logInfo("All logs cleared");
}

void LogManager::createLogsFolder() {
    Serial.println("LogManager::createLogsFolder()");
    
    if (!SPIFFS.exists("/logs")) {
        // SPIFFS doesn't have mkdir, but directories are created automatically
        // when files are written to them
        File temp = SPIFFS.open("/logs/temp", "w");
        if (temp) {
            temp.close();
            SPIFFS.remove("/logs/temp");
            Serial.println("Logs folder created");
        }
    }
}

void LogManager::openNewLogFile() {
    Serial.println("LogManager::openNewLogFile()");
    
    if (currentLogFile) {
        currentLogFile.close();
    }
    
    String fileName = getCurrentLogFileName();
    currentLogFile = SPIFFS.open(fileName, "a");
    
    if (!currentLogFile) {
        Serial.println("Failed to open log file");
    } else {
        currentLogSize = currentLogFile.size();
        Serial.print("Opened log file: ");
        Serial.println(fileName);
    }
}

void LogManager::rotateLogFiles() {
    Serial.println("LogManager::rotateLogFiles()");
    
    if (currentLogFile) {
        currentLogFile.close();
    }
    
    currentLogNumber++;
    currentLogSize = 0;
    openNewLogFile();
    cleanOldLogFiles();
}

void LogManager::cleanOldLogFiles() {
    Serial.println("LogManager::cleanOldLogFiles()");
    
    // Count log files and remove oldest if more than maxLogFiles
    int fileCount = 0;
    int lowestLogNumber = currentLogNumber;
    
    File root = SPIFFS.open("/logs");
    if (root && root.isDirectory()) {
        File file = root.openNextFile();
        while (file) {
            String fileName = file.name();
            if (fileName.endsWith(".log")) {
                fileCount++;
                int logNum = fileName.substring(fileName.lastIndexOf('/') + 1).toInt();
                if (logNum < lowestLogNumber) {
                    lowestLogNumber = logNum;
                }
            }
            file = root.openNextFile();
        }
        root.close();
    }
    
    if (fileCount > maxLogFiles) {
        String oldestFile = "/logs/" + String(lowestLogNumber) + ".log";
        SPIFFS.remove(oldestFile);
        Serial.print("Removed old log file: ");
        Serial.println(oldestFile);
    }
}

String LogManager::getCurrentLogFileName() {
    Serial.println("LogManager::getCurrentLogFileName()");
    String result = "/logs/" + String(currentLogNumber) + ".log";
    Serial.print("LogManager::getCurrentLogFileName() returning: ");
    Serial.println(result);
    return result;
}

void LogManager::writeLogEntry(const String& level, const String& message) {
    // Serial.print("LogManager::writeLogEntry(");
    // Serial.print(level);
    // Serial.print(", ");
    // Serial.print(message);
    // Serial.println(")");
    
    if (!currentLogFile) {
        openNewLogFile();
    }
    
    if (!currentLogFile) {
        return;
    }
    
    String timestamp = getTimestamp();
    String logEntry = timestamp + " [" + level + "] " + message + "\n";
    
    currentLogFile.print(logEntry);
    currentLogFile.flush();
    currentLogSize += logEntry.length();
    
    // Check if we need to rotate the log file
    if (currentLogSize >= maxLogSize) {
        rotateLogFiles();
    }
}

String LogManager::getTimestamp() {
    // Serial.println("LogManager::getTimestamp()");
    
    char timestamp[24];
    sprintf(timestamp, "%04d-%02d-%02d %02d:%02d:%02d",
            year(), month(), day(), hour(), minute(), second());
    
    String result = String(timestamp);
    // Serial.print("LogManager::getTimestamp() returning: ");
    // Serial.println(result);
    return result;
}