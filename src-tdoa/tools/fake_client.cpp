// g++ -o fake_client fake_client.cpp SignalDataService.cpp -std=c++11
// clang++ fake_client.cpp SignalDataService.cpp -o fake_client -std=c++11

// run the below command on the raspberry pi if the socket connection fails
// sudo iptables -A INPUT -p tcp --dport 8080 -j ACCEPT

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <cstring>

#include "SignalDataService.hpp"

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;

    // create socket file descriptor
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);

    // convert IP addy from text to binary form
    if(inet_pton(AF_INET, "172.19.172.124", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        return -1;
    }

    // attempt server connection
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection Failed" << std::endl;
        return -1;
    }

    std::cout << "Connected to the server successfully." << std::endl;

    // temp time series data
    // int data[] = {10, 20, 30, 40, 50};
    // int dataCount = sizeof(data) / sizeof(data[0]);

    // time_series_req_t req = {1, dataCount, data};

    // SignalDataService service;
    // buffer_t serialized = service.time_series_serialize_req(req);

    // send(sock, serialized.data, serialized.length, 0);
    
    // std::cout << "Time series data sent successfully." << std::endl;

    // // receive response
    // char buffer[1024] = {0};
    // int valread = read(sock, buffer, 1024);
    // if (valread > 0) {
    //     time_series_resp_t response;
    //     buffer_t responseBuffer = {buffer, static_cast<size_t>(valread)};
    //     service.time_series_deserialize_resp(responseBuffer, &response);
    //     std::cout << "Message received from server: id=" << response.id << ", status=" << response.status << std::endl;
    // }
    while(true) {

        std::vector<int16_t> fixedSamples(FIXED_SAMPLE_COUNT, 0);  // init to zero
        time_series_req_t req = {1000000000, int16_t(fixedSamples.size()), fixedSamples.data()};

        SignalDataService service;
        buffer_t serialized = service.time_series_serialize_req(req);
        send(sock, serialized.data, serialized.length, 0);
        std::cout << "Time series data sent successfully." << std::endl;

        // receive response
        char buffer[1024] = {0};
        int valread = read(sock, buffer, 1024);
        if (valread > 0) {
            time_series_resp_t response;
            buffer_t responseBuffer = {buffer, static_cast<size_t>(valread)};
            service.time_series_deserialize_resp(responseBuffer, &response);
            std::cout << "Message received from server: id=" << response.id << ", status=" << response.status << std::endl;
        }
    }

    close(sock);

    return 0;
}
