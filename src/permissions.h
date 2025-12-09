 #ifndef PERMISSIONS_H
#define PERMISSIONS_H

#include <string>
#include <map>

class PermissionManager {
private:
    std::map<std::string, int> userPermissions;
    
public:
    void setPermission(const std::string& username, int level);
    int getPermission(const std::string& username);
    bool hasReadAccess(const std::string& username);
    bool hasWriteAccess(const std::string& username);
    bool hasDeleteAccess(const std::string& username);
};

#endif
