#ifndef STREAM_SERVER_CORE_HPP
#define STREAM_SERVER_CORE_HPP

#include <functional>
#include <thread>
#include <vector>
#include <cstring>
#include <string> 

#include "SignalDataService.hpp"

class StreamServerCore {
public:
    explicit StreamServerCore(int port);
    ~StreamServerCore();
    StreamServerCore(const StreamServerCore&) = delete;
    StreamServerCore& operator=(const StreamServerCore&) = delete;
    void acceptConnections();
    void clientHandler(int clientSocketDescriptor, std::string clientIP);
    void sendResponse(int clientSocketDescriptor, const time_series_resp_t& response);

private:
    int serverSocketDescriptor;
    int port;
    std::vector<std::string> raspberryPiIPs; 
};

#endif // STREAM_SERVER_CORE_HPP
