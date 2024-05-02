#include "SignalProcessor.hpp"
#include "CrossCorrelation.hpp"
#include <iostream>
#include <chrono>
#include <csignal>
#include <sqlite3.h>
#include <cstdlib>
#include <thread>


const char* database = "/home/calvin/repos/CESE-CellularIntrusionDetection-S2024/src/flask/cid.db";

SignalProcessor::SignalProcessor() {}

SignalProcessor::~SignalProcessor() {}

void SignalProcessor::addPiData(int piId, const PiData& data) {
    std::lock_guard<std::mutex> lock(dataMutex);
    timestampData[data.timestamp][piId] = data;
    if (isDataReady(data.timestamp)) {
        std::cout << "Data is ready to process!" << std::endl;
        dataCondition.notify_one();
    }
}

sqlite3* create_connection(const char* db_file) {
    sqlite3* db;
    int rc = sqlite3_open(db_file, &db);
    if (rc != SQLITE_OK) {
        std::cerr << "Error opening database: " << sqlite3_errmsg(db) << std::endl;
        return nullptr;
    }
    return db;
}

// insert a new coordinate into coordinates table
bool insert_coordinate(sqlite3* db, int x, int y) {
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO coordinates(x, y) VALUES(?,?)";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_int(stmt, 1, x);
    sqlite3_bind_int(stmt, 2, y);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Failed to execute statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}

bool SignalProcessor::isDataReady(const long long timestamp) {
    return timestampData[timestamp].size() == 3; // Assuming data must be collected from 3 Pis
}

void SignalProcessor::processData() {
    std::unique_lock<std::mutex> lock(dataMutex);
    while (true) { // Replace with a condition to stop processing
        dataCondition.wait(lock, [this] {
            return std::any_of(timestampData.begin(), timestampData.end(), [this](auto& pair) {
                return isDataReady(pair.first);
            });
        });

        for (auto it = timestampData.begin(); it != timestampData.end(); ) {
            if (isDataReady(it->first)) {
                processTimestampData(it->first);
                it = timestampData.erase(it);
            } else {
                ++it;
            }
        }
    }
}

//std::vector<double> SignalProcessor::getSamplesForKey(const std::map<int, PiData>& data, int key) {
////    // Check if the key exists in the map
////    auto it = data.find(key);
////    if (it != data.end()) {
////        // Key found, check if there are at least three samples
////        const PiData& piData = it->second;
////        if (piData.samples.size() >= 3) {
////            return {piData.samples[0], piData.samples[1], piData.samples[2]};
////        }
////    }
////    // Return an empty vector if the key doesn't exist or not enough samples
////    return {};
//
//    auto it = data.find(key);
//    if (it != data.end()) {
//        const PiData& piData = it->second;
//        if (piData.samples.size() >= 3) {
//        // Explicitly cast short int to double to avoid narrowing conversion warnings
//            return {static_cast<double>(piData.samples[0]),
//                    static_cast<double>(piData.samples[1]),
//                    static_cast<double>(piData.samples[2])};
//        }
//    }
//    return {}; // Return an empty vector if the key doesn't exist or not enough samples
//}


void SignalProcessor::processTimestampData(const long long timestamp) {
    std::cout << std::flush;
    std::cout << "Processing data for timestamp: " << timestamp << std::endl;
//    std::vector<std::vector<double>> Rx = { {0, 0}, {D, 0}, {0.25 * D, D} };
    std::vector<std::vector<double>> Rx = { {0.115, 0.16}, {3.173, 6.62}, {6.105, 0.295} };
    //750*0.115/750

    // Calculate Distance using Cross Correlation Delays
//    double delay_12 = 12;
//    double delay_13 = 13;
    std::cout << "Beginning Cross Correlation and TDOA!" << std::endl;
    //std::vector<double> iq_data1, iq_data2, iq_data3;


    auto iter = timestampData[timestamp].begin();
    std::vector<short int>& iq_data1 = iter->second.samples;
    ++iter;
    std::vector<short int>& iq_data2 = iter->second.samples;
    ++iter;
    std::vector<short int>& iq_data3 = iter->second.samples;

    double sample_rate = 2000000; // Example sample rate
    double speed_of_light = 299792458; // Speed of light in m/s

    DelayValues delays = time_difference(iq_data1, iq_data2, iq_data3, sample_rate, speed_of_light);


    //std::vector<double> samples = getSamplesForKey(timestampData[timestamp], timestamp);

    // Defining Constants
//    double sample_rate    = 2000000;
//    double speed_of_light = 299792458;
    // Call Time-Difference Function
    //DelayValues delays = time_difference(samples[0], samples[1], samples[2], sample_rate, speed_of_light);
    //DelayValues delays = time_difference(iq_data1, iq_data2, iq_data3, sample_rate, speed_of_light);
    std::vector<double> distances = calculate_distance(delays.delay_12, delays.delay_13);
    std::vector<std::vector<double>> position = tdoa_calculation(distances[0], distances[1], Rx);
    std::cout << "Device located at: " <<  position[0][0] << ", " <<  position[0][1] << std::endl;

    // connect to db
    sqlite3* db = create_connection(database);
    if (!db) {
        std::cout << "DB connection failed." << std::endl;
    };

//    std::random_device rd;
//    std::mt19937 gen(rd());
//    std::uniform_int_distribution<> distrib(0, 750);


    int x = 0;
    int y = 0;

    if (position[0][0] >= 0 && position[0][1] >= 0) {
        x = 7.5*int(position[0][0])/6;
        y = 7.5*int(position[0][1])/6;
    } else {
        //std::cout << "Values are negative." << std::endl;
        x = 7.5*int(position[0][0])/6;
        y = 7.5*int(position[0][1])/6 + 30;
    }
    if (!insert_coordinate(db, x, y)) {
        std::cerr << "Insert failed." << std::endl;
    }
    std::cout << "Values sent to DB: " << x << " " << y << std::endl;

    sqlite3_close(db);
//    performCrossCorrelationAndTDOA(timestampData[timestamp]);
}

//void SignalProcessor::performCrossCorrelationAndTDOA(const std::map<int, PiData>& data) {
//    std::cout << "Beginning Cross Correlation and TDOA!" << std::endl;
//    std::vector<std::vector<double>> Rx = { {0, 0}, {D, 0}, {0.25 * D, D} };
//    // Calculate Distance using Cross Correlation Delays
//    double delay_12 = 12;
//    double delay_13 = 13;
//    std::vector<double> distances = calculate_distance(delay_12, delay_13);
//    std::vector<std::vector<double>> position = tdoa_calculation(distances[0], distances[1], Rx);
//    std::cout << "Device located at: " <<  position[0][0] << ", " <<  position[0][1] << std::endl;
//    //std::raise(SIGINT);
//}

long long SignalProcessor::getTime() {
    // Get the current system time
    auto now = std::chrono::system_clock::now();

    // Convert system time to time since epoch (seconds)
    auto time_since_epoch = now.time_since_epoch();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(time_since_epoch);

    // Convert seconds to integer
    long long int_time = seconds.count();
    return int_time;
}
