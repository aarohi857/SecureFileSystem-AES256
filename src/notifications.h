 #ifndef NOTIFICATIONS_H
#define NOTIFICATIONS_H

#include <string>
#include <vector>
#include <queue>

struct Notification {
    std::string message;
    std::string type;
    std::string timestamp;
    bool isRead;
};

class NotificationManager {
private:
    std::queue<Notification> notificationQueue;
    std::vector<Notification> notificationHistory;
    int maxNotifications;
    
    std::string getCurrentTimestamp();
    
public:
    NotificationManager();
    void sendNotification(const std::string& message, const std::string& type);
    void displayUnreadNotifications();
    void displayAllNotifications();
    void markAsRead();
    void clearNotifications();
    int getUnreadCount();
};

#endif
