#ifndef SIGNAL_DATA_SERVICE_HPP
#define SIGNAL_DATA_SERVICE_HPP

#include <cstddef>
#include <cstdint>

struct time_series_req_t {
    long unsigned int id;       // clock time when first sample rx in data array
    int count;                  // number of data points
    int16_t* data;              // pointer to an array of integers (int16_t)
};

struct time_series_resp_t {
    long unsigned int id;       // corresponding request identifier
    int status;                 // status of the operation (OK, ERROR, etc.)
};

struct buffer_t {
    char* data;
    size_t length;
};

const int FIXED_SAMPLE_COUNT = 16384;

class SignalDataService {
public:
    SignalDataService();
    ~SignalDataService(); 
    buffer_t time_series_serialize_req(const time_series_req_t& req);
    buffer_t time_series_serialize_resp(const time_series_resp_t* resp);
    void time_series_deserialize_req(const buffer_t buffer, time_series_req_t* req);
    void time_series_deserialize_resp(const buffer_t buffer, time_series_resp_t* resp);
    void deallocateSerializedData();
    

private:
    // vars to store serialized data
    char* serializedData;
    size_t serializedDataSize;
    double samplePowerdBm;
};

#endif
