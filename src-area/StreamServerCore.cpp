#include "StreamServerCore.hpp"
#include "SignalDataService.hpp"
#include "RxwFunctions.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <cmath>
#include <unistd.h>
#include <memory>
#include <stdexcept>
#include <cstring>
#include <arpa/inet.h>

StreamServerCore::StreamServerCore(int port) 
    : port(port) {
    serverSocketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocketDescriptor == -1) {
        throw std::runtime_error("Could not create socket");
    }

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(serverSocketDescriptor, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) == -1) {
        close(serverSocketDescriptor);
        throw std::runtime_error("Could not bind the address");
    }

    if (listen(serverSocketDescriptor, 10) == -1) {
        close(serverSocketDescriptor);
        throw std::runtime_error("Could not set the backlog");
    }

    std::cout << "Server started on port " << port << std::endl; 

}

StreamServerCore::~StreamServerCore() {
    close(serverSocketDescriptor);
}

void StreamServerCore::acceptConnections() {
    std::cout << "Waiting for incoming connections..." << std::endl;

    while (true) {
        sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        int clientSocketDescriptor = accept(serverSocketDescriptor, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if (clientSocketDescriptor == -1) {
            throw std::runtime_error("Could not accept client");
        }

        // Convert IP to a readable format
        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, INET_ADDRSTRLEN);

        std::cout << "Connection accepted from " << clientIP << std::endl;
        std::thread(&StreamServerCore::clientHandler, this, clientSocketDescriptor, std::string(clientIP)).detach();
    }
}

void StreamServerCore::sendResponse(int clientSocketDescriptor, const time_series_resp_t& response) {
    SignalDataService service;
    buffer_t serialized = service.time_series_serialize_resp(&response);
    send(clientSocketDescriptor, serialized.data, serialized.length, 0);
    service.deallocateSerializedData();  
}

void StreamServerCore::clientHandler(int clientSocketDescriptor, std::string clientIP) { 
    std::cout << "Client handler thread started for IP: " << clientIP << std::endl; 

    while (true) {
        try {

            // -------------------
            // READ SOCKET PACKET
            // -------------------

            size_t totalBytesRead = 0;
            char buffer[32808]; //buffers are a fixed size
            const int expectedBytes = sizeof(int) * 2 + sizeof(int16_t) * FIXED_SAMPLE_COUNT; // Adjust accordingly

            try
            {
        
                while (totalBytesRead < expectedBytes) {
                    ssize_t bytesRead = read(clientSocketDescriptor, buffer + totalBytesRead, sizeof(buffer) - totalBytesRead);
                    if (bytesRead <= 0) {
                        if (bytesRead == 0) {
                            std::cerr << "Client disconnected." << std::endl;
                        } else {
                            std::cerr << "Read error." << std::endl;
                        }
                        close(clientSocketDescriptor);
                        return; 
                    }
                totalBytesRead += bytesRead;
                }
            }
            catch(const std::exception& e)
            {
                std::cout << "Error in reading socket packet: " << e.what() << std::endl;
                std::cerr << e.what() << '\n';
            }

            // -------------------
            // DESERIALIZE REQUEST
            // -------------------
            SignalDataService serializer;
            time_series_req_t request;
            try
            {
                buffer_t receivedBuffer = {buffer, static_cast<size_t>(totalBytesRead)};
                serializer.time_series_deserialize_req(receivedBuffer, &request);
            }
            catch(const std::exception& e)
            {
                std::cout << "Error in buffer allocatoin: " << e.what() << std::endl;
                std::cerr << e.what() << '\n';
            }
            
            /*
            // Print the incoming Data
            std::cout << "rx data from " << clientIP << ": time=" << request.id << ", count=" << request.count << ", data2=";
            // print beginning of data array
            for (int i = 0; i < 4; ++i) {
                std::cout << request.data[i] << " ";
            }
            //*/

            // ------------------
            // PROCESSESS SAMPLES
            // ------------------
            //*
            try
            {
                // Convert to vector
                std::vector<int16_t> dataVector = vectorizeSamples(request.data);
                // Calculate Sample Power
                double totalPowerdBm = processSamplePower(dataVector);
                // Detect Signal Above Threshold
                bool signalDetected = thresholdDetection(totalPowerdBm/NUM_SAMPLES);
                
                if (signalDetected){
                    // share data with main thread
                }
                else {
                    // Clear collected data
                }

                #ifdef DEBUG
                // Print Threshold Detection
                if (signalDetected){
                    
                    std::cout << std::flush;
                    std::cout << "Signal Detected: " << std::endl;
                       
                }
                else {
                    std::cout << std::flush;
                    std::cout << "Average Power: " << totalPowerdBm/NUM_SAMPLES << " dBm" << std::endl;
                }
                #endif 

            }
            catch(const std::exception& e)
            {
                std::cout << "Error in processing samples: " << e.what() << std::endl;
                std::cerr << e.what() << '\n';
            }
            //*/

            // -------------------
            // SEND RESPONSE
            // -------------------
            time_series_resp_t response = {request.id, 200};  
            sendResponse(clientSocketDescriptor, response);
            // TODO vary response based on error messages

        } catch (const std::exception& e) {
            std::cerr << "Exception in client handler: " << e.what() << std::endl;
        }
    }
    close(clientSocketDescriptor);

    std::cout << "Client handler thread finished" << std::endl; 
}