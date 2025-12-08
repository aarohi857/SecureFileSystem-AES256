/*
 * Secure File Management System - Web Version
 * Simple HTTP Server + Beautiful Web UI
 * Features: Authentication, Encryption, Access Control
 */

#include <windows.h>
#include <winsock2.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "auth.h"
#include "fileops.h"
#include "encryption.h"

#pragma comment(lib, "ws2_32.lib")

// Global variables
std::string currentUser = "";

// Function declarations
void startServer();
void handleRequest(SOCKET clientSocket, const std::string& request);
std::string readFile(const std::string& filename);
void sendResponse(SOCKET client, int statusCode, const std::string& contentType, const std::string& body);
void openBrowser();

// Entry point
int main() {
    // Initialize systems
    initAuthSystem();
    initEncryptionSystem();
    initFileOpsSystem();
    
    std::cout << "======================================" << std::endl;
    std::cout << "  Secure File Management System" << std::endl;
    std::cout << "======================================" << std::endl;
    std::cout << "\nStarting server on port 8080...\n" << std::endl;
    
    // Start server
    startServer();
    
    return 0;
}

// Open default browser
void openBrowser() {
    std::cout << "Opening browser..." << std::endl;
    ShellExecute(NULL, "open", "http://localhost:8080", NULL, NULL, SW_SHOWNORMAL);
}

// Start HTTP Server
void startServer() {
    WSADATA wsaData;
    SOCKET serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    int clientAddrLen = sizeof(clientAddr);
    
    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "ERROR: WSAStartup failed" << std::endl;
        system("pause");
        return;
    }
    
    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "ERROR: Socket creation failed" << std::endl;
        WSACleanup();
        system("pause");
        return;
    }
    
    // Setup server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8080);
    
    // Bind socket
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "ERROR: Bind failed (Port 8080 may be in use)" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        system("pause");
        return;
    }
    
    // Listen
    if (listen(serverSocket, 10) == SOCKET_ERROR) {
        std::cerr << "ERROR: Listen failed" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        system("pause");
        return;
    }
    
    std::cout << "✓ Server started successfully!" << std::endl;
    std::cout << "✓ Access URL: http://localhost:8080" << std::endl;
    std::cout << "\n[Press Ctrl+C to stop server]\n" << std::endl;
    
    // Open browser after 1 second
    Sleep(1000);
    openBrowser();
    
    // Accept connections loop
    while (true) {
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        
        if (clientSocket == INVALID_SOCKET) {
            continue;
        }
        
        // Receive request
        char buffer[8192] = {0};
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        
        if (bytesReceived > 0) {
            std::string request(buffer);
            handleRequest(clientSocket, request);
        }
        
        closesocket(clientSocket);
    }
    
    closesocket(serverSocket);
    WSACleanup();
}

// Handle HTTP request
void handleRequest(SOCKET clientSocket, const std::string& request) {
    std::istringstream iss(request);
    std::string method, path, version;
    iss >> method >> path >> version;
    
    std::cout << "[" << method << "] " << path << std::endl;
    
    // Serve static files
    if (path == "/" || path == "/index.html") {
        std::string html = readFile("index.html");
        if (!html.empty()) {
            sendResponse(clientSocket, 200, "text/html", html);
        } else {
            std::string error = "<h1>Error: index.html not found</h1><p>Make sure index.html is in the same folder as the .exe</p>";
            sendResponse(clientSocket, 404, "text/html", error);
        }
    }
    else if (path == "/style.css") {
        std::string css = readFile("style.css");
        if (!css.empty()) {
            sendResponse(clientSocket, 200, "text/css", css);
        } else {
            sendResponse(clientSocket, 404, "text/plain", "style.css not found");
        }
    }
    else if (path == "/script.js") {
        std::string js = readFile("script.js");
        if (!js.empty()) {
            sendResponse(clientSocket, 200, "application/javascript", js);
        } else {
            sendResponse(clientSocket, 404, "text/plain", "script.js not found");
        }
    }
    // API: Login
    else if (path == "/api/login" && method == "POST") {
        size_t bodyPos = request.find("\r\n\r\n");
        if (bodyPos != std::string::npos) {
            std::string body = request.substr(bodyPos + 4);
            
            // Extract username
            size_t usernamePos = body.find("\"username\"");
            std::string username = "";
            if (usernamePos != std::string::npos) {
                size_t start = body.find(":", usernamePos) + 3;
                size_t end = body.find("\"", start);
                username = body.substr(start, end - start);
            }
            
            // Extract password
            size_t passwordPos = body.find("\"password\"");
            std::string password = "";
            if (passwordPos != std::string::npos) {
                size_t start = body.find(":", passwordPos) + 3;
                size_t end = body.find("\"", start);
                password = body.substr(start, end - start);
            }
            
            if (!username.empty() && !password.empty()) {
                bool success = loginUser(username, password);
                
                // Auto-register if user doesn't exist
                if (!success && !userExists(username)) {
                    if (registerUser(username, username + "@secure.com", password)) {
                        success = loginUser(username, password);
                    }
                }
                
                if (success) {
                    currentUser = username;
                    std::string response = "{\"success\":true,\"message\":\"Login successful\"}";
                    sendResponse(clientSocket, 200, "application/json", response);
                    std::cout << "  → User logged in: " << username << std::endl;
                } else {
                    std::string response = "{\"success\":false,\"message\":\"Invalid credentials\"}";
                    sendResponse(clientSocket, 200, "application/json", response);
                }
            }
        }
    }
    // API: Stats
    else if (path == "/api/stats") {
        std::vector<FileMetadata> files = listUserFiles(currentUser);
        long long totalSize = 0;
        for (const auto& file : files) {
            totalSize += file.fileSize;
        }
        
        std::stringstream ss;
        ss << "{\"totalFiles\":" << files.size() 
           << ",\"encryptedFiles\":" << files.size()
           << ",\"sharedFiles\":0"
           << ",\"totalSize\":" << (totalSize / 1024) << "}";
        sendResponse(clientSocket, 200, "application/json", ss.str());
    }
    // API: Files list
    else if (path.find("/api/files") == 0) {
        std::vector<FileMetadata> files = listUserFiles(currentUser);
        std::stringstream ss;
        ss << "{\"files\":[";
        for (size_t i = 0; i < files.size(); i++) {
            if (i > 0) ss << ",";
            ss << "{\"id\":" << files[i].fileId 
               << ",\"name\":\"" << files[i].originalName << "\""
               << ",\"size\":" << files[i].fileSize
               << ",\"date\":\"" << files[i].uploadDate << "\"}";
        }
        ss << "]}";
        sendResponse(clientSocket, 200, "application/json", ss.str());
    }
    // API: Logout
    else if (path == "/api/logout" && method == "POST") {
        std::cout << "  → User logged out: " << currentUser << std::endl;
        currentUser = "";
        std::string response = "{\"success\":true}";
        sendResponse(clientSocket, 200, "application/json", response);
    }
    // 404
    else {
        sendResponse(clientSocket, 404, "text/plain", "404 - Not Found");
    }
}

// Read file
std::string readFile(const std::string& filename) {
    std::ifstream file(filename.c_str(), std::ios::binary);
    if (!file.is_open()) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Send HTTP response
void sendResponse(SOCKET client, int statusCode, const std::string& contentType, const std::string& body) {
    std::stringstream response;
    
    std::string statusText = (statusCode == 200) ? "OK" : 
                             (statusCode == 404) ? "Not Found" : "Error";
    
    response << "HTTP/1.1 " << statusCode << " " << statusText << "\r\n";
    response << "Content-Type: " << contentType << "; charset=utf-8\r\n";
    response << "Content-Length: " << body.length() << "\r\n";
    response << "Access-Control-Allow-Origin: *\r\n";
    response << "Connection: close\r\n";
    response << "\r\n";
    response << body;
    
    std::string responseStr = response.str();
    send(client, responseStr.c_str(), static_cast<int>(responseStr.length()), 0);
}