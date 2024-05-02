#include "SignalDataService.hpp"
#include <cstring>
#include <stdexcept>
#include <cstdint>
#include <iostream>
#include <vector>

SignalDataService::SignalDataService() : serializedData(nullptr), serializedDataSize(0){

}

SignalDataService::~SignalDataService() {
    deallocateSerializedData();
}

buffer_t SignalDataService::time_series_serialize_req(const time_series_req_t& req) {
    // calc buffer size using sizeof(int16_t) for the data segment
    size_t buffer_size = sizeof(req.id) + sizeof(req.count) + sizeof(int16_t) * req.count;
    std::vector<char> serializedData(buffer_size);  // use a vector to manage memory

    size_t offset = 0;
    memcpy(serializedData.data() + offset, &req.id, sizeof(req.id));
    offset += sizeof(req.id);
    memcpy(serializedData.data() + offset, &req.count, sizeof(req.count));
    offset += sizeof(req.count);
    memcpy(serializedData.data() + offset, req.data, sizeof(int16_t) * req.count);

    buffer_t buffer;
    buffer.data = serializedData.data();  // .data() returns a pointer to the vector's buffer
    buffer.length = serializedData.size();  // Use the size of the vector

    return buffer;  // Note: Ensure that the lifetime of serializedData vector covers the usage of buffer
}

buffer_t SignalDataService::time_series_serialize_resp(const time_series_resp_t* resp) {
    // calc size needed for serialization
    size_t buffer_size = sizeof(resp->id) + sizeof(resp->status);

    // allocate mem for serialized data
    serializedData = new char[buffer_size];

    // copy data into serialized data buffer
    size_t offset = 0;
    memcpy(serializedData + offset, &resp->id, sizeof(resp->id));
    offset += sizeof(resp->id);
    memcpy(serializedData + offset, &resp->status, sizeof(resp->status));

    serializedDataSize = buffer_size;

    // create object to hold serialized data
    buffer_t buffer;
    buffer.data = serializedData;
    buffer.length = serializedDataSize;

    return buffer;
}

void SignalDataService::deallocateSerializedData() {
    // deallocate mem for serialized data if not nullptr
    if (serializedData != nullptr) {
        delete[] serializedData;
        serializedData = nullptr;
        serializedDataSize = 0;
    }
}

void SignalDataService::time_series_deserialize_req(const buffer_t buffer, time_series_req_t* req) {
    // check if buffer length is sufficient for deserialization
    if (buffer.length < sizeof(int) * 2) {
        throw std::runtime_error("Buffer size insufficient for data array deserialization");
    }      

    // extract data from buffer and assign it to the request object
    size_t offset = 0;
    memcpy(&req->id, buffer.data + offset, sizeof(req->id));
    offset += sizeof(req->id);
    memcpy(&req->count, buffer.data + offset, sizeof(req->count));
    offset += sizeof(req->count);

    req->data = new(std::nothrow) int16_t[req->count];
    if (!req->data) {
        throw std::bad_alloc();
    }
    
    // check if buffer length is sufficient to hold the data array
    size_t expectedDataSize = sizeof(int16_t) * req->count;
    if (buffer.length - offset < expectedDataSize) {
        // Clean up memory if buffer length is insufficient
        delete[] req->data;
        throw std::runtime_error("Buffer size insufficient for data array deserialization");
    }

    // copy data array from buffer to request object
    memcpy(req->data, buffer.data + offset, expectedDataSize);
}

void SignalDataService::time_series_deserialize_resp(const buffer_t buffer, time_series_resp_t* resp) {
    // check if buffer length is sufficient for deserialization
    if (buffer.length < sizeof(int) * 2) {
        throw std::runtime_error("Buffer size insufficient for deserialization");
    }

    // extract data from buffer and assign it to the request object
    size_t offset = 0;
    memcpy(&resp->id, buffer.data + offset, sizeof(resp->id));
    offset += sizeof(resp->id);
    memcpy(&resp->status, buffer.data + offset, sizeof(resp->status));
}
