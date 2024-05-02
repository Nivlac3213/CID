// Compile server using the following command:
// g++ -o serverApp main.cpp StreamServerCore.cpp SignalDataService.cpp RxwFunctions.cpp -lpthread
// clang++ -o serverApp main.cpp StreamServerCore.cpp SignalDataService.cpp -lpthread -std=c++11
// ^^if running on macOS

#include <iostream>
#include <stdexcept>
#include <vector>
#include <thread>
#include <string>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "StreamServerCore.hpp"

// deprecated
// std::vector<std::string> getRaspberryPiIPs(const std::string& scriptPath) {
//     std::vector<std::string> ips;
//     char buffer[128];
//     std::string command = scriptPath + " 2>&1"; // cmd to execute
//     FILE* pipe = popen(command.c_str(), "r"); // open the script as a process

//     if (!pipe) throw std::runtime_error("popen() failed!");
//     try {
//         while (fgets(buffer, sizeof buffer, pipe) != NULL) {
//             std::string line = buffer;
//             if (line.find("The IP address for MAC") != std::string::npos) {
//                 // extract IP address
//                 std::size_t start = line.find_last_of(' ') + 1;
//                 std::string ip = line.substr(start, line.length() - start - 1);
//                 ips.push_back(ip);
//             }
//         }
//     } catch (...) {
//         pclose(pipe);
//         throw;
//     }
//     pclose(pipe);
//     return ips;
// }

int main() {
    try {

        // deprecated
        // std::string scriptPath = "./get_raspberry_pi_ips.sh"; 
        // const std::vector<std::string>& raspberryPiIps = getRaspberryPiIPs(scriptPath);
        // for (const auto& ip : raspberryPiIps) {
        //     std::cout << "Found Raspberry Pi at IP: " << ip << std::endl;
        // }

        // initialize server on port 8080
        StreamServerCore server(8080); 

        // start accepting connections
        server.acceptConnections();
    } catch (const std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
