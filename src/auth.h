/*
 * Authentication Module - Header File
 * Handles user registration, login, password hashing, 2FA
 */

#ifndef AUTH_H
#define AUTH_H

#include <string>

// Initialize authentication system
void initAuthSystem();

// User registration
bool registerUser(const std::string& username, const std::string& email, const std::string& password);

// User login
bool loginUser(const std::string& username, const std::string& password);

// Password validation
bool validatePassword(const std::string& password);

// Password hashing
std::string hashPassword(const std::string& password);

// Verify hashed password
bool verifyPassword(const std::string& password, const std::string& hashedPassword);

// Generate 2FA secret
std::string generate2FASecret();

// Verify 2FA code
bool verify2FACode(const std::string& secret, const std::string& code);

// Check if user exists
bool userExists(const std::string& username);

// Get user ID
int getUserId(const std::string& username);

// Session management
std::string createSession(const std::string& username);
bool validateSession(const std::string& sessionToken);
void destroySession(const std::string& sessionToken);

#endif // AUTH_H