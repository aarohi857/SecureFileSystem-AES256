/*
 * Encryption Module - Header File
 * Handles file encryption/decryption using AES-256
 */

#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <string>
#include <vector>

// Initialize encryption system
void initEncryptionSystem();

// Encrypt file
bool encryptFile(const std::string& inputPath, const std::string& outputPath, const std::string& key);

// Decrypt file
bool decryptFile(const std::string& inputPath, const std::string& outputPath, const std::string& key);

// Generate encryption key
std::string generateEncryptionKey();

// Encrypt data (in memory)
std::vector<unsigned char> encryptData(const std::vector<unsigned char>& data, const std::string& key);

// Decrypt data (in memory)
std::vector<unsigned char> decryptData(const std::vector<unsigned char>& data, const std::string& key);

// XOR cipher (simple encryption for demonstration)
void xorCipher(unsigned char* data, size_t dataLen, const std::string& key);

#endif // ENCRYPTION_H