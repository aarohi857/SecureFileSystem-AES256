#ifndef BACKUP_H
#define BACKUP_H

#include <string>
#include <vector>

class BackupManager {
private:
    std::string backupDirectory;
    std::vector<std::string> backupHistory;
    
public:
    BackupManager();
    bool createBackup(const std::string& filename);
    bool restoreBackup(const std::string& backupName);
    void listBackups();
    bool deleteBackup(const std::string& backupName);
    void autoBackup();
};

#endif 
