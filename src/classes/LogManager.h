#ifndef LOG_MANAGER_H
#define LOG_MANAGER_H

#include <Arduino.h>
#include <SPIFFS.h>

class LogManager {
public:
    LogManager();
    ~LogManager();
    
    void begin();
    void logInfo(const String& message);
    void logError(const String& message);
    String getLastLogContent();
    String getLastLogLines(int numLines);
    void clearAllLogs();

private:
    int currentLogNumber;
    File currentLogFile;
    unsigned long currentLogSize;
    const unsigned long maxLogSize = 1048576; // 1MB
    const int maxLogFiles = 50;
    const int startingLogNumber = 1000;
    
    void createLogsFolder();
    void openNewLogFile();
    void rotateLogFiles();
    void cleanOldLogFiles();
    String getCurrentLogFileName();
    void writeLogEntry(const String& level, const String& message);
    String getTimestamp();
};

#endif