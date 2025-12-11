#include "notifications.h"
#include <iostream>
#include <ctime>
#include <sstream>
#include <iomanip>

NotificationManager::NotificationManager() {
    maxNotifications = 50;
}

std::string NotificationManager::getCurrentTimestamp() {
    time_t now = time(0);
    struct tm timeinfo;
    localtime_s(&timeinfo, &now);
    
    std::stringstream ss;
    ss << std::put_time(&timeinfo, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

void NotificationManager::sendNotification(const std::string& message, const std::string& type) {
    Notification notif;
    notif.message = message;
    notif.type = type;
    notif.timestamp = getCurrentTimestamp();
    notif.isRead = false;
    
    notificationQueue.push(notif);
    notificationHistory.push_back(notif);
    
    if (notificationHistory.size() > maxNotifications) {
        notificationHistory.erase(notificationHistory.begin());
    }
    
    std::cout << "\n[NEW NOTIFICATION]" << std::endl;
    std::cout << "Type: " << type << std::endl;
    std::cout << "Message: " << message << std::endl;
    std::cout << "Time: " << notif.timestamp << std::endl;
}

void NotificationManager::displayUnreadNotifications() {
    if (notificationQueue.empty()) {
        std::cout << "\nNo unread notifications!" << std::endl;
        return;
    }
    
    std::cout << "\n========== Unread Notifications ==========" << std::endl;
    
    std::queue<Notification> tempQueue = notificationQueue;
    int count = 1;
    
    while (!tempQueue.empty()) {
        Notification notif = tempQueue.front();
        tempQueue.pop();
        
        std::cout << "\n[" << count++ << "] " << notif.type << std::endl;
        std::cout << "    " << notif.message << std::endl;
        std::cout << "    Time: " << notif.timestamp << std::endl;
    }
    
    std::cout << "==========================================" << std::endl;
}

void NotificationManager::displayAllNotifications() {
    if (notificationHistory.empty()) {
        std::cout << "\nNo notifications in history!" << std::endl;
        return;
    }
    
    std::cout << "\n========== Notification History ==========" << std::endl;
    
    for (size_t i = 0; i < notificationHistory.size(); i++) {
        Notification notif = notificationHistory[i];
        
        std::cout << "\n[" << (i + 1) << "] " << notif.type;
        if (notif.isRead) {
            std::cout << " (READ)";
        } else {
            std::cout << " (UNREAD)";
        }
        std::cout << std::endl;
        std::cout << "    " << notif.message << std::endl;
        std::cout << "    Time: " << notif.timestamp << std::endl;
    }
    
    std::cout << "==========================================" << std::endl;
    std::cout << "Total notifications: " << notificationHistory.size() << std::endl;
}

void NotificationManager::markAsRead() {
    if (notificationQueue.empty()) {
        std::cout << "\nNo unread notifications to mark!" << std::endl;
        return;
    }
    
    int count = notificationQueue.size();
    
    while (!notificationQueue.empty()) {
        notificationQueue.pop();
    }
    
    for (auto& notif : notificationHistory) {
        if (!notif.isRead) {
            notif.isRead = true;
        }
    }
    
    std::cout << "\n" << count << " notification(s) marked as read!" << std::endl;
}

void NotificationManager::clearNotifications() {
    notificationQueue = std::queue<Notification>();
    notificationHistory.clear();
    
    std::cout << "\nAll notifications cleared successfully!" << std::endl;
}

int NotificationManager::getUnreadCount() {
    return notificationQueue.size();
}
