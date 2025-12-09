#include "permissions.h"
#include <iostream>

void PermissionManager::setPermission(const std::string& username, int level) {
    userPermissions[username] = level;
    std::cout << "Permission level " << level << " set for user: " << username << std::endl;
}

int PermissionManager::getPermission(const std::string& username) {
    if (userPermissions.find(username) != userPermissions.end()) {
        return userPermissions[username];
    }
    return 0;
}

bool PermissionManager::hasReadAccess(const std::string& username) {
    return getPermission(username) >= 1;
}

bool PermissionManager::hasWriteAccess(const std::string& username) {
    return getPermission(username) >= 2;
}

bool PermissionManager::hasDeleteAccess(const std::string& username) {
    return getPermission(username) >= 3;
}
