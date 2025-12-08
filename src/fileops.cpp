/*
 * File Operations Module - Implementation
 * Complete file management with encryption and access control
 */

#include "fileops.h"
#include "encryption.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <windows.h>

// File database
static std::map<int, FileMetadata> fileDatabase;
static std::map<std::string, std::vector<int>> userFiles; // username -> file IDs
static std::map<int, std::map<std::string, std::string>> filePermissions; // fileId -> (username -> permission)
static int nextFileId = 1;

// Dangerous file extensions (malware detection)
static std::vector<std::string> dangerousExtensions = {
    ".exe", ".bat", ".cmd", ".com", ".scr", ".vbs", ".js", ".jar"
};

// Initialize system
void initFileOpsSystem() {
    CreateDirectory("uploads", NULL);
    CreateDirectory("database", NULL);
    
    // Load file metadata from database
    std::ifstream inFile("database/files.dat");
    if (inFile.is_open()) {
        std::string line;
        while (std::getline(inFile, line)) {
            std::istringstream iss(line);
            FileMetadata meta;
            iss >> meta.fileId >> meta.originalName >> meta.encryptedName 
                >> meta.owner >> meta.fileSize >> meta.fileType;
            
            fileDatabase[meta.fileId] = meta;
            userFiles[meta.owner].push_back(meta.fileId);
            
            if (meta.fileId >= nextFileId) {
                nextFileId = meta.fileId + 1;
            }
        }
        inFile.close();
    }
    
    std::cout << "File operations system initialized. Files: " << fileDatabase.size() << std::endl;
}

// Get current timestamp
std::string getCurrentTimestamp() {
    time_t now = time(0);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&now));
    return std::string(buffer);
}

// Get file size
long long getFileSize(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) return 0;
    long long size = file.tellg();
    file.close();
    return size;
}

// Get file extension
std::string getFileExtension(const std::string& filename) {
    size_t pos = filename.find_last_of('.');
    if (pos == std::string::npos) return "";
    return filename.substr(pos);
}

// Sanitize filename (buffer overflow protection)
std::string sanitizeFilename(const std::string& filename) {
    std::string sanitized = filename;
    
    // Remove path separators
    sanitized.erase(std::remove(sanitized.begin(), sanitized.end(), '/'), sanitized.end());
    sanitized.erase(std::remove(sanitized.begin(), sanitized.end(), '\\'), sanitized.end());
    
    // Limit length
    if (sanitized.length() > 255) {
        sanitized = sanitized.substr(0, 255);
    }
    
    // Remove special characters
    for (char& c : sanitized) {
        if (c < 32 || c == '<' || c == '>' || c == ':' || c == '"' || c == '|' || c == '?' || c == '*') {
            c = '_';
        }
    }
    
    return sanitized;
}

// Validate file (malware detection simulation)
bool validateFile(const std::string& filePath) {
    // Check file extension
    std::string ext = getFileExtension(filePath);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    for (const auto& dangerousExt : dangerousExtensions) {
        if (ext == dangerousExt) {
            std::cerr << "File validation failed: Dangerous extension detected - " << ext << std::endl;
            return false;
        }
    }
    
    // Check file size (max 100MB)
    long long fileSize = getFileSize(filePath);
    if (fileSize > 100 * 1024 * 1024) {
        std::cerr << "File validation failed: File too large (max 100MB)" << std::endl;
        return false;
    }
    
    if (fileSize == 0) {
        std::cerr << "File validation failed: File is empty" << std::endl;
        return false;
    }
    
    // Simulate malware signature check
    // In production, integrate with antivirus library
    std::cout << "File validation passed: " << filePath << std::endl;
    return true;
}

// Upload file
std::string uploadFile(const std::string& filePath, const std::string& username) {
    // Validate file
    if (!validateFile(filePath)) {
        return "Upload failed: File validation error";
    }
    
    // Extract filename
    size_t pos = filePath.find_last_of("\\/");
    std::string filename = (pos == std::string::npos) ? filePath : filePath.substr(pos + 1);
    filename = sanitizeFilename(filename);
    
    // Generate encrypted filename
    srand(time(NULL) + rand());
    std::stringstream ss;
    ss << "enc_" << time(NULL) << "_" << rand() << getFileExtension(filename);
    std::string encryptedFilename = ss.str();
    
    // Generate encryption key
    std::string encKey = generateEncryptionKey();
    
    // Encrypt and save file
    std::string encryptedPath = "uploads/" + encryptedFilename;
    if (!encryptFile(filePath, encryptedPath, encKey)) {
        return "Upload failed: Encryption error";
    }
    
    // Create metadata
    FileMetadata meta;
    meta.fileId = nextFileId++;
    meta.originalName = filename;
    meta.encryptedName = encryptedFilename;
    meta.owner = username;
    meta.fileSize = getFileSize(filePath);
    meta.fileType = getFileExtension(filename);
    meta.uploadDate = getCurrentTimestamp();
    meta.isEncrypted = true;
    meta.encryptionKey = encKey;
    
    // Store in database
    fileDatabase[meta.fileId] = meta;
    userFiles[username].push_back(meta.fileId);
    
    // Set owner permissions
    filePermissions[meta.fileId][username] = "owner";
    
    // Save metadata to file
    std::ofstream outFile("database/files.dat", std::ios::app);
    if (outFile.is_open()) {
        outFile << meta.fileId << " " << meta.originalName << " " 
                << meta.encryptedName << " " << meta.owner << " " 
                << meta.fileSize << " " << meta.fileType << std::endl;
        outFile.close();
    }
    
    // Log activity
    std::ofstream logFile("database/activity.log", std::ios::app);
    if (logFile.is_open()) {
        logFile << getCurrentTimestamp() << " - User: " << username 
                << " uploaded file: " << filename << " (ID: " << meta.fileId << ")" << std::endl;
        logFile.close();
    }
    
    std::stringstream result;
    result << "File uploaded successfully!\n\n"
           << "File ID: " << meta.fileId << "\n"
           << "Filename: " << filename << "\n"
           << "Size: " << meta.fileSize << " bytes\n"
           << "Status: Encrypted and secured";
    
    return result.str();
}

// Download file
bool downloadFile(int fileId, const std::string& username, const std::string& outputPath) {
    // Check if file exists
    auto it = fileDatabase.find(fileId);
    if (it == fileDatabase.end()) {
        std::cerr << "Download failed: File not found" << std::endl;
        return false;
    }
    
    // Check access permission
    if (!checkFileAccess(fileId, username, "read")) {
        std::cerr << "Download failed: Access denied" << std::endl;
        return false;
    }
    
    FileMetadata meta = it->second;
    std::string encryptedPath = "uploads/" + meta.encryptedName;
    
    // Decrypt file
    if (!decryptFile(encryptedPath, outputPath, meta.encryptionKey)) {
        std::cerr << "Download failed: Decryption error" << std::endl;
        return false;
    }
    
    // Log activity
    std::ofstream logFile("database/activity.log", std::ios::app);
    if (logFile.is_open()) {
        logFile << getCurrentTimestamp() << " - User: " << username 
                << " downloaded file: " << meta.originalName << " (ID: " << fileId << ")" << std::endl;
        logFile.close();
    }
    
    std::cout << "File downloaded successfully: " << outputPath << std::endl;
    return true;
}

// Delete file
bool deleteFile(int fileId, const std::string& username) {
    // Check if file exists
    auto it = fileDatabase.find(fileId);
    if (it == fileDatabase.end()) {
        std::cerr << "Delete failed: File not found" << std::endl;
        return false;
    }
    
    // Check if user is owner
    if (it->second.owner != username) {
        std::cerr << "Delete failed: Only owner can delete" << std::endl;
        return false;
    }
    
    FileMetadata meta = it->second;
    std::string encryptedPath = "uploads/" + meta.encryptedName;
    
    // Delete physical file
    DeleteFile(encryptedPath.c_str());
    
    // Remove from database
    fileDatabase.erase(fileId);
    
    // Remove from user files
    auto& files = userFiles[username];
    files.erase(std::remove(files.begin(), files.end(), fileId), files.end());
    
    // Remove permissions
    filePermissions.erase(fileId);
    
    // Log activity
    std::ofstream logFile("database/activity.log", std::ios::app);
    if (logFile.is_open()) {
        logFile << getCurrentTimestamp() << " - User: " << username 
                << " deleted file: " << meta.originalName << " (ID: " << fileId << ")" << std::endl;
        logFile.close();
    }
    
    std::cout << "File deleted successfully: " << meta.originalName << std::endl;
    return true;
}

// Share file
bool shareFile(int fileId, const std::string& ownerUsername, const std::string& targetUsername, const std::string& permission) {
    // Check if file exists
    auto it = fileDatabase.find(fileId);
    if (it == fileDatabase.end()) {
        std::cerr << "Share failed: File not found" << std::endl;
        return false;
    }
    
    // Check if user is owner
    if (it->second.owner != ownerUsername) {
        std::cerr << "Share failed: Only owner can share" << std::endl;
        return false;
    }
    
    // Set permission
    filePermissions[fileId][targetUsername] = permission;
    
    // Add file to target user's file list
    userFiles[targetUsername].push_back(fileId);
    
    // Log activity
    std::ofstream logFile("database/activity.log", std::ios::app);
    if (logFile.is_open()) {
        logFile << getCurrentTimestamp() << " - User: " << ownerUsername 
                << " shared file ID " << fileId << " with " << targetUsername 
                << " (permission: " << permission << ")" << std::endl;
        logFile.close();
    }
    
    std::cout << "File shared successfully with " << targetUsername << std::endl;
    return true;
}

// Get file metadata
FileMetadata getFileMetadata(int fileId) {
    return fileDatabase[fileId];
}

// List user files
std::vector<FileMetadata> listUserFiles(const std::string& username) {
    std::vector<FileMetadata> result;
    
    for (int fileId : userFiles[username]) {
        if (fileDatabase.find(fileId) != fileDatabase.end()) {
            result.push_back(fileDatabase[fileId]);
        }
    }
    
    return result;
}

// Check file access permission
bool checkFileAccess(int fileId, const std::string& username, const std::string& permission) {
    // Check if file exists
    auto it = fileDatabase.find(fileId);
    if (it == fileDatabase.end()) {
        return false;
    }
    
    // Owner has all permissions
    if (it->second.owner == username) {
        return true;
    }
    
    // Check explicit permissions
    auto permIt = filePermissions.find(fileId);
    if (permIt == filePermissions.end()) {
        return false;
    }
    
    auto userPermIt = permIt->second.find(username);
    if (userPermIt == permIt->second.end()) {
        return false;
    }
    
    std::string userPerm = userPermIt->second;
    
    // Permission hierarchy: owner > write > read
    if (permission == "read") {
        return userPerm == "read" || userPerm == "write" || userPerm == "owner";
    } else if (permission == "write") {
        return userPerm == "write" || userPerm == "owner";
    } else if (permission == "owner") {
        return userPerm == "owner";
    }
    
    return false;
}