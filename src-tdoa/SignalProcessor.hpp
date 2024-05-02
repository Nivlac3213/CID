#ifndef SIGNAL_PROCESSOR_HPP
#define SIGNAL_PROCESSOR_HPP

#include <vector>
#include <mutex>
#include <condition_variable>
#include <map>
#include <cstdint>
#include <algorithm>
#include "RxwFunctions.hpp"
#include "TDOA.hpp"

const int nRx = 3; // Number of receivers
const int nTx = 1; // Number of emitters
const int dim = 2; // Number of dimensions for the problem

// Struct to hold time series data from a single Pi
struct PiData {
    long long timestamp;
    std::vector<int16_t> samples;
};

//std::map<int, PiData> dataMap;

class SignalProcessor {
public:
    SignalProcessor();
    ~SignalProcessor();

    // Add data from a specific Pi and timestamp
    void addPiData(int piId, const PiData& data);

    // Main processing loop to be run in a separate thread
    void processData();
    long long getTime();

private:
    std::mutex dataMutex;
    std::condition_variable dataCondition;
    std::map<long long, std::map<int, PiData>> timestampData; // Maps timestamp to Pi data

    // Helper functions
    bool isDataReady(const long long timestamp);
    void processTimestampData(const long long timestamp);
//    std::vector<double> getSamplesForKey(const std::map<int, PiData>& data, int key);

    // Signal processing functions
//    void performCrossCorrelationAndTDOA(const std::map<int, PiData>& data);
};

#endif // SIGNAL_PROCESSOR_HPP
