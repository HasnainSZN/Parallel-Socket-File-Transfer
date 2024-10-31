#include <iostream>
#include <fstream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <chrono>   // For timing
#include <omp.h>    // For OpenMP

#pragma comment(lib, "ws2_32.lib") // Link with ws2_32.lib

#define PORT 8080
#define BUFFER_SIZE 1024

void handleClient(SOCKET clientSocket) {
    char buffer[BUFFER_SIZE];

    // Receive the file name
    int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);
    if (bytesReceived <= 0) {
        std::cerr << "Failed to receive file name." << std::endl;
        closesocket(clientSocket);
        return;
    }

    std::string fileName(buffer, bytesReceived);
    std::cout << "Receiving file: " << fileName << std::endl;

    // Open file to write
    std::ofstream outFile(fileName, std::ios::binary);
    if (!outFile) {
        std::cerr << "Failed to open file for writing." << std::endl;
        closesocket(clientSocket);
        return;
    }

    // Receive file data
    while ((bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0)) > 0) {
        outFile.write(buffer, bytesReceived);
    }

    outFile.close();
    std::cout << "File received successfully: " << fileName << std::endl;

    // Close client socket
    closesocket(clientSocket);
}

int main() {
    WSADATA wsaData;
    SOCKET serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    int addrLen = sizeof(clientAddr);

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed." << std::endl;
        return 1;
    }

    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed." << std::endl;
        WSACleanup();
        return 1;
    }

    // Set up the server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // Bind the socket
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Listen for connections
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server listening on port " << PORT << std::endl;

    // --- Serial Execution ---
    auto startSerial = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 5; ++i) {  
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrLen);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Accept failed." << std::endl;
            continue;
        }
        handleClient(clientSocket);  // Handle each client serially
    }

    auto endSerial = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> serialDuration = endSerial - startSerial;
    std::cout << "Serial Execution Time: " << serialDuration.count() << " seconds" << std::endl;

    // --- Parallel Execution using OpenMP ---
    auto startParallel = std::chrono::high_resolution_clock::now();

    #pragma omp parallel for num_threads(4)
    for (int i = 0; i < 5; ++i) {
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrLen);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Accept failed." << std::endl;
            continue;
        }
        handleClient(clientSocket);  // Each client is handled in parallel
    }

    auto endParallel = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> parallelDuration = endParallel - startParallel;
    std::cout << "Parallel Execution Time: " << parallelDuration.count() << " seconds" << std::endl;

    // Clean up
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
