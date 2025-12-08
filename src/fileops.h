/*
 * File Operations Module - Header File
 * Handles file upload, download, sharing, metadata
 */

#ifndef FILEOPS_H
#define FILEOPS_H

#include <string>
#include <vector>

// File metadata structure
struct FileMetadata {
    int fileId;
    std::string originalName;
    std::string encryptedName;
    std::string owner;
    long long fileSize;
    std::string fileType;
    std::string uploadDate;
    bool isEncrypted;
    std::string encryptionKey;
};

// Initialize file operations system
void initFileOpsSystem();

// Upload file
std::string uploadFile(const std::string& filePath, const std::string& username);

// Download file
bool downloadFile(int fileId, const std::string& username, const std::string& outputPath);

// Delete file
bool deleteFile(int fileId, const std::string& username);

// Share file with user
bool shareFile(int fileId, const std::string& ownerUsername, const std::string& targetUsername, const std::string& permission);

// Get file metadata
FileMetadata getFileMetadata(int fileId);

// List user files
std::vector<FileMetadata> listUserFiles(const std::string& username);

// Check file access permission
bool checkFileAccess(int fileId, const std::string& username, const std::string& permission);

// Get file size
long long getFileSize(const std::string& filePath);

// Get file extension
std::string getFileExtension(const std::string& filename);

// Sanitize filename (buffer overflow protection)
std::string sanitizeFilename(const std::string& filename);

// Validate file (malware detection simulation)
bool validateFile(const std::string& filePath);

#endif // FILEOPS_H