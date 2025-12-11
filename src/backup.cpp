#include "backup.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <ctime>
#include <sstream>
#include <iomanip>

namespace fs = std::filesystem;

BackupManager::BackupManager() {
    backupDirectory = "backups/";
    
    if (!fs::exists(backupDirectory)) {
        fs::create_directory(backupDirectory);
        std::cout << "Backup directory created successfully" << std::endl;
    }
}

std::string getCurrentTimestamp() {
    time_t now = time(0);
    struct tm timeinfo;
    localtime_s(&timeinfo, &now);
    
    std::stringstream ss;
    ss << std::put_time(&timeinfo, "%Y%m%d_%H%M%S");
    return ss.str();
}

bool BackupManager::createBackup(const std::string& filename) {
    try {
        if (!fs::exists(filename)) {
            std::cerr << "Error: Source file does not exist!" << std::endl;
            return false;
        }
        
        std::string timestamp = getCurrentTimestamp();
        std::string backupName = backupDirectory + "backup_" + timestamp + "_" + 
                                fs::path(filename).filename().string();
        
        fs::copy_file(filename, backupName, fs::copy_options::overwrite_existing);
        
        backupHistory.push_back(backupName);
        
        std::cout << "Backup created successfully: " << backupName << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Backup failed: " << e.what() << std::endl;
        return false;
    }
}

bool BackupManager::restoreBackup(const std::string& backupName) {
    try {
        std::string fullPath = backupDirectory + backupName;
        
        if (!fs::exists(fullPath)) {
            std::cerr << "Error: Backup file not found!" << std::endl;
            return false;
        }
        
        std::string originalName = backupName.substr(backupName.find_last_of("_") + 1);
        
        fs::copy_file(fullPath, originalName, fs::copy_options::overwrite_existing);
        
        std::cout << "Backup restored successfully: " << originalName << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Restore failed: " << e.what() << std::endl;
        return false;
    }
}

void BackupManager::listBackups() {
    std::cout << "\n========== Available Backups ==========" << std::endl;
    
    int count = 0;
    for (const auto& entry : fs::directory_iterator(backupDirectory)) {
        std::cout << ++count << ". " << entry.path().filename().string() << std::endl;
    }
    
    if (count == 0) {
        std::cout << "No backups found!" << std::endl;
    }
    
    std::cout << "=======================================" << std::endl;
}

bool BackupManager::deleteBackup(const std::string& backupName) {
    try {
        std::string fullPath = backupDirectory + backupName;
        
        if (fs::remove(fullPath)) {
            std::cout << "Backup deleted successfully: " << backupName << std::endl;
            return true;
        } else {
            std::cerr << "Error: Backup file not found!" << std::endl;
            return false;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Delete failed: " << e.what() << std::endl;
        return false;
    }
}

void BackupManager::autoBackup() {
    std::cout << "Starting automatic backup..." << std::endl;
    
    for (const auto& entry : fs::directory_iterator("uploads/")) {
        if (entry.is_regular_file()) {
            createBackup(entry.path().string());
        }
    }
    
    std::cout << "Automatic backup completed!" << std::endl;
} 
