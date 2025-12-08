/*
 * Authentication Module - Implementation
 * Complete user authentication with password hashing
 */

#include "auth.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <ctime>
#include <cstdlib>
#include <windows.h>
#include <wincrypt.h>

// Simple user database (in production, use SQLite)
struct User {
    int id;
    std::string username;
    std::string email;
    std::string passwordHash;
    std::string twoFASecret;
    bool is2FAEnabled;
};

static std::map<std::string, User> userDatabase;
static std::map<std::string, std::string> sessionTokens;
static int nextUserId = 1;

// Initialize system
void initAuthSystem() {
    // Create database directory if not exists
    CreateDirectory("database", NULL);
    
    // Load users from file (simple file-based storage)
    std::ifstream inFile("database/users.dat");
    if (inFile.is_open()) {
        std::string line;
        while (std::getline(inFile, line)) {
            std::istringstream iss(line);
            User user;
            iss >> user.id >> user.username >> user.email >> user.passwordHash;
            userDatabase[user.username] = user;
            if (user.id >= nextUserId) {
                nextUserId = user.id + 1;
            }
        }
        inFile.close();
    }
    
    std::cout << "Authentication system initialized. Users loaded: " << userDatabase.size() << std::endl;
}

// Simple hash function (SHA-256 simulation - in production use proper crypto library)
std::string hashPassword(const std::string& password) {
    // NOTE: This is simplified. In production, use bcrypt or similar
    unsigned long hash = 5381;
    for (char c : password) {
        hash = ((hash << 5) + hash) + c;
    }
    
    // Add salt
    hash = hash ^ 0xDEADBEEF;
    
    std::stringstream ss;
    ss << std::hex << hash;
    return ss.str();
}

// Verify password
bool verifyPassword(const std::string& password, const std::string& hashedPassword) {
    return hashPassword(password) == hashedPassword;
}

// Validate password strength
bool validatePassword(const std::string& password) {
    if (password.length() < 8) return false;
    
    bool hasUpper = false, hasLower = false, hasDigit = false, hasSpecial = false;
    
    for (char c : password) {
        if (isupper(c)) hasUpper = true;
        if (islower(c)) hasLower = true;
        if (isdigit(c)) hasDigit = true;
        if (ispunct(c)) hasSpecial = true;
    }
    
    return hasUpper && hasLower && hasDigit && hasSpecial;
}

// Register user
bool registerUser(const std::string& username, const std::string& email, const std::string& password) {
    // Input validation
    if (username.empty() || email.empty() || password.empty()) {
        std::cerr << "Registration failed: Empty fields" << std::endl;
        return false;
    }
    
    // Check if user already exists
    if (userDatabase.find(username) != userDatabase.end()) {
        std::cerr << "Registration failed: Username already exists" << std::endl;
        return false;
    }
    
    // Validate password strength
    if (!validatePassword(password)) {
        std::cerr << "Registration failed: Weak password" << std::endl;
        return false;
    }
    
    // Sanitize input (buffer overflow protection)
    if (username.length() > 50 || email.length() > 100 || password.length() > 128) {
        std::cerr << "Registration failed: Input too long" << std::endl;
        return false;
    }
    
    // Create new user
    User newUser;
    newUser.id = nextUserId++;
    newUser.username = username;
    newUser.email = email;
    newUser.passwordHash = hashPassword(password);
    newUser.is2FAEnabled = false;
    
    userDatabase[username] = newUser;
    
    // Save to file
    std::ofstream outFile("database/users.dat", std::ios::app);
    if (outFile.is_open()) {
        outFile << newUser.id << " " << newUser.username << " " 
                << newUser.email << " " << newUser.passwordHash << std::endl;
        outFile.close();
    }
    
    std::cout << "User registered successfully: " << username << std::endl;
    return true;
}

// Login user
bool loginUser(const std::string& username, const std::string& password) {
    // Input sanitization (buffer overflow protection)
    if (username.length() > 50 || password.length() > 128) {
        std::cerr << "Login failed: Input too long" << std::endl;
        return false;
    }
    
    // Check if user exists
    auto it = userDatabase.find(username);
    if (it == userDatabase.end()) {
        std::cerr << "Login failed: User not found" << std::endl;
        return false;
    }
    
    // Verify password
    if (!verifyPassword(password, it->second.passwordHash)) {
        std::cerr << "Login failed: Incorrect password" << std::endl;
        return false;
    }
    
    std::cout << "Login successful: " << username << std::endl;
    return true;
}

// Check if user exists
bool userExists(const std::string& username) {
    return userDatabase.find(username) != userDatabase.end();
}

// Get user ID
int getUserId(const std::string& username) {
    auto it = userDatabase.find(username);
    if (it != userDatabase.end()) {
        return it->second.id;
    }
    return -1;
}

// Create session token
std::string createSession(const std::string& username) {
    // Generate random session token
    srand(time(NULL));
    std::stringstream ss;
    ss << username << "_" << rand() << "_" << time(NULL);
    std::string token = hashPassword(ss.str());
    
    sessionTokens[token] = username;
    return token;
}

// Validate session
bool validateSession(const std::string& sessionToken) {
    return sessionTokens.find(sessionToken) != sessionTokens.end();
}

// Destroy session
void destroySession(const std::string& sessionToken) {
    sessionTokens.erase(sessionToken);
}

// Generate 2FA secret (simplified)
std::string generate2FASecret() {
    srand(time(NULL));
    std::stringstream ss;
    for (int i = 0; i < 16; i++) {
        ss << (rand() % 10);
    }
    return ss.str();
}

// Verify 2FA code (simplified TOTP implementation)
bool verify2FACode(const std::string& secret, const std::string& code) {
    // In production, implement proper TOTP algorithm
    // This is simplified for demonstration
    time_t currentTime = time(NULL) / 30; // 30-second window
    std::stringstream ss;
    ss << secret << currentTime;
    std::string expected = hashPassword(ss.str()).substr(0, 6);
    
    return code == expected;
}