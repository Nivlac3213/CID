#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>

int main() {
    // Create a socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return 1;
    }

    // Bind the socket to a specific IP address and port
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(8080);
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        std::cerr << "Failed to bind socket" << std::endl;
        return 1;
    }

    // Listen for incoming connections
    if (listen(serverSocket, 3) < 0) {
        std::cerr << "Failed to listen for connections" << std::endl;
        return 1;
    }

    // Accept incoming connections
    int clientSocket;
    sockaddr_in clientAddress{};
    int clientAddressSize = sizeof(clientAddress);
    if ((clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, (socklen_t*)&clientAddressSize)) < 0) {
        std::cerr << "Failed to accept connection" << std::endl;
        return 1;
    }

    // Handle client requests
    // TODO: Implement your server logic here

    // Close the sockets
    close(clientSocket);
    close(serverSocket);

    return 0;
}
