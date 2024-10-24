#include <iostream>
#include <fstream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib") // Link with ws2_32.lib

#define SERVER_IP "127.0.0.1" // Change to server IP if needed
#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
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
