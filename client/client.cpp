#include <iostream>
#include <fstream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <chrono>  // For timing
#include <windows.h>  // For system info
#include <intrin.h>  // For CPU info

#pragma comment(lib, "ws2_32.lib") // Link with ws2_32.lib

#define SERVER_IP "127.0.0.1" // Change to server IP if needed
#define PORT 8080
#define BUFFER_SIZE 1024


// Function to get CPU info
std::string GetCPUInfo() {
    char CPUBrandString[0x40];
    int CPUInfo[4] = { -1 };
    __cpuid(CPUInfo, 0x80000002);
    memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));
    __cpuid(CPUInfo, 0x80000003);
    memcpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
    __cpuid(CPUInfo, 0x80000004);
    memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
    return std::string(CPUBrandString);
}


// Function to get system memory info
std::string GetMemoryInfo() {
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    DWORDLONG totalPhysMem = memInfo.ullTotalPhys;
    return std::to_string(totalPhysMem / (1024 * 1024 * 1024)) + " GB";
}

int main() {


    // Print hardware info
    std::cout << "Hardware Information:" << std::endl;
    std::cout << "CPU: " << GetCPUInfo() << std::endl;
    std::cout << "RAM: " << GetMemoryInfo() << std::endl;


    // Start timing
    auto startTime = std::chrono::high_resolution_clock::now();


    
    WSADATA wsaData;
    SOCKET clientSocket;
    struct sockaddr_in serverAddr;
    char buffer[BUFFER_SIZE];

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed." << std::endl;
        return 1;
    }

    // Create socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed." << std::endl;
        WSACleanup();
        return 1;
    }

    // Set up the server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);

    // Connect to server
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connection to server failed." << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // Input file name
    std::string fileName;
    std::cout << "Enter the file name to send: ";
    std::getline(std::cin, fileName);

    // Send the file name
    send(clientSocket, fileName.c_str(), fileName.size(), 0);

    // Open the file for reading
    std::ifstream inFile(fileName, std::ios::binary);
    if (!inFile) {
        std::cerr << "Failed to open file for reading." << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // Send the file data
    while (inFile.read(buffer, BUFFER_SIZE)) {
        send(clientSocket, buffer, inFile.gcount(), 0);
    }
    // Send any remaining bytes
    if (inFile.gcount() > 0) {
        send(clientSocket, buffer, inFile.gcount(), 0);
    }

    std::cout << "File sent: " << fileName << std::endl;

    // Close the file and socket
    inFile.close();
    closesocket(clientSocket);
    WSACleanup();
    return 0;
}
