#include <iostream>
#include <fstream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>

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

    while (true) {
        // Accept a connection
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrLen);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Accept failed." << std::endl;
            continue; // Continue to the next iteration
        }

        // Create a new thread to handle the client
        std::thread(handleClient, clientSocket).detach();
    }

    // Clean up
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
