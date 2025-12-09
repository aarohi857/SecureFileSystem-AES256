#include "logging.h"
#include <iostream>
#include <fstream>
#include <ctime>

void LogManager::logActivity(const std::string& username, const std::string& action) {
    std::ofstream logFile("activity_log.txt", std::ios::app);
    
    if (logFile.is_open()) {
        time_t now = time(0);
        char* dt = ctime(&now);
        
        logFile << "[" << dt << "] User: " << username 
                << " | Action: " << action << std::endl;
        
        logFile.close();
        std::cout << "Activity logged successfully!" << std::endl;
    } else {
        std::cerr << "Error opening log file!" << std::endl;
    }
}

void LogManager::viewLogs() {
    std::ifstream logFile("activity_log.txt");
    std::string line;
    
    if (logFile.is_open()) {
        std::cout << "\n=== Activity Logs ===" << std::endl;
        while (getline(logFile, line)) {
            std::cout << line << std::endl;
        }
        logFile.close();
    } else {
        std::cout << "No logs found!" << std::endl;
    }
}