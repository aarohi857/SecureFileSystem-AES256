/*
 * Encryption Module - Implementation
 * File encryption using XOR cipher (AES-256 simulation)
 */

#include "encryption.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <cstdlib>
#include <windows.h>

// Master encryption key (in production, store securely)
static std::string masterKey = "SecureFileMgmt2024!@#$%^&*()_+";

// Initialize encryption system
void initEncryptionSystem() {
    // Create uploads directory
    CreateDirectory("uploads", NULL);
    CreateDirectory("database", NULL);
    std::cout << "Encryption system initialized" << std::endl;
}

// Generate random encryption key
std::string generateEncryptionKey() {
    srand(static_cast<unsigned int>(time(NULL)) + rand());
    std::stringstream ss;
    
    const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!@#$%^&*";
    int charsetSize = sizeof(charset) - 1;
    
    for (int i = 0; i < 32; i++) {
        ss << charset[rand() % charsetSize];
    }
    
    return ss.str();
}

// XOR cipher implementation (simple but effective for demonstration)
// NOTE: In production, use OpenSSL or Crypto++ for AES-256
void xorCipher(unsigned char* data, size_t dataLen, const std::string& key) {
    if (dataLen == 0 || key.empty()) {
        std::cerr << "xorCipher: Invalid input - dataLen=" << dataLen << ", keyLen=" << key.length() << std::endl;
        return;
    }
    
    size_t keyLen = key.length();
    for (size_t i = 0; i < dataLen; i++) {
        data[i] ^= key[i % keyLen];
    }
}

// Encrypt file
bool encryptFile(const std::string& inputPath, const std::string& outputPath, const std::string& key) {
    std::cout << "Starting encryption: " << inputPath << std::endl;
    
    // Open input file
    std::ifstream inFile(inputPath.c_str(), std::ios::binary | std::ios::ate);
    if (!inFile.is_open()) {
        std::cerr << "Failed to open input file: " << inputPath << std::endl;
        std::cerr << "Error: File may not exist or access denied" << std::endl;
        return false;
    }
    
    // Get file size
    std::streamsize fileSize = inFile.tellg();
    inFile.seekg(0, std::ios::beg);
    
    if (fileSize <= 0) {
        std::cerr << "File is empty or error reading size: " << inputPath << std::endl;
        inFile.close();
        return false;
    }
    
    std::cout << "File size: " << fileSize << " bytes" << std::endl;
    
    // Read file content into buffer
    std::vector<unsigned char> buffer(fileSize);
    if (!inFile.read(reinterpret_cast<char*>(buffer.data()), fileSize)) {
        std::cerr << "Failed to read file content" << std::endl;
        inFile.close();
        return false;
    }
    inFile.close();
    
    std::cout << "File read successfully, buffer size: " << buffer.size() << std::endl;
    
    // Validate buffer
    if (buffer.empty()) {
        std::cerr << "Buffer is empty after reading file" << std::endl;
        return false;
    }
    
    // Create combined key
    std::string combinedKey = key + masterKey;
    if (combinedKey.empty()) {
        std::cerr << "Combined encryption key is empty!" << std::endl;
        return false;
    }
    
    std::cout << "Encrypting with key length: " << combinedKey.length() << std::endl;
    
    // Encrypt data using XOR cipher
    xorCipher(buffer.data(), buffer.size(), combinedKey);
    
    std::cout << "Encryption completed, writing to: " << outputPath << std::endl;
    
    // Create output directory if needed
    size_t pos = outputPath.find_last_of("\\/");
    if (pos != std::string::npos) {
        std::string dir = outputPath.substr(0, pos);
        CreateDirectory(dir.c_str(), NULL);
    }
    
    // Write encrypted data
    std::ofstream outFile(outputPath.c_str(), std::ios::binary);
    if (!outFile.is_open()) {
        std::cerr << "Failed to create output file: " << outputPath << std::endl;
        std::cerr << "Check if 'uploads' directory exists and has write permissions" << std::endl;
        return false;
    }
    
    outFile.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
    
    if (!outFile.good()) {
        std::cerr << "Error writing encrypted data" << std::endl;
        outFile.close();
        return false;
    }
    
    outFile.close();
    
    // Verify file was created
    std::ifstream verifyFile(outputPath.c_str(), std::ios::binary);
    if (!verifyFile.is_open()) {
        std::cerr << "Verification failed: encrypted file not found" << std::endl;
        return false;
    }
    verifyFile.close();
    
    std::cout << "File encrypted successfully: " << outputPath << std::endl;
    return true;
}

// Decrypt file
bool decryptFile(const std::string& inputPath, const std::string& outputPath, const std::string& key) {
    std::cout << "Starting decryption: " << inputPath << std::endl;
    
    // Open encrypted file
    std::ifstream inFile(inputPath.c_str(), std::ios::binary | std::ios::ate);
    if (!inFile.is_open()) {
        std::cerr << "Failed to open encrypted file: " << inputPath << std::endl;
        return false;
    }
    
    // Get file size
    std::streamsize fileSize = inFile.tellg();
    inFile.seekg(0, std::ios::beg);
    
    if (fileSize <= 0) {
        std::cerr << "Encrypted file is empty: " << inputPath << std::endl;
        inFile.close();
        return false;
    }
    
    std::cout << "Encrypted file size: " << fileSize << " bytes" << std::endl;
    
    // Read encrypted content
    std::vector<unsigned char> buffer(fileSize);
    if (!inFile.read(reinterpret_cast<char*>(buffer.data()), fileSize)) {
        std::cerr << "Failed to read encrypted content" << std::endl;
        inFile.close();
        return false;
    }
    inFile.close();
    
    if (buffer.empty()) {
        std::cerr << "Buffer is empty after reading encrypted file" << std::endl;
        return false;
    }
    
    // Create combined key
    std::string combinedKey = key + masterKey;
    
    std::cout << "Decrypting with key length: " << combinedKey.length() << std::endl;
    
    // Decrypt data (XOR is symmetric, so same function works)
    xorCipher(buffer.data(), buffer.size(), combinedKey);
    
    std::cout << "Decryption completed, writing to: " << outputPath << std::endl;
    
    // Write decrypted data
    std::ofstream outFile(outputPath.c_str(), std::ios::binary);
    if (!outFile.is_open()) {
        std::cerr << "Failed to create decrypted file: " << outputPath << std::endl;
        return false;
    }
    
    outFile.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
    
    if (!outFile.good()) {
        std::cerr << "Error writing decrypted data" << std::endl;
        outFile.close();
        return false;
    }
    
    outFile.close();
    
    std::cout << "File decrypted successfully: " << outputPath << std::endl;
    return true;
}

// Encrypt data in memory
std::vector<unsigned char> encryptData(const std::vector<unsigned char>& data, const std::string& key) {
    if (data.empty()) {
        std::cerr << "encryptData: Input data is empty" << std::endl;
        return std::vector<unsigned char>();
    }
    
    std::vector<unsigned char> encrypted = data;
    std::string combinedKey = key + masterKey;
    xorCipher(encrypted.data(), encrypted.size(), combinedKey);
    return encrypted;
}

// Decrypt data in memory
std::vector<unsigned char> decryptData(const std::vector<unsigned char>& data, const std::string& key) {
    if (data.empty()) {
        std::cerr << "decryptData: Input data is empty" << std::endl;
        return std::vector<unsigned char>();
    }
    
    std::vector<unsigned char> decrypted = data;
    std::string combinedKey = key + masterKey;
    xorCipher(decrypted.data(), decrypted.size(), combinedKey);
    return decrypted;
}