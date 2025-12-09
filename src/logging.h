#ifndef LOGGING_H
#define LOGGING_H

#include <string>

class LogManager {
public:
    void logActivity(const std::string& username, const std::string& action);
    void viewLogs();
};

#endif