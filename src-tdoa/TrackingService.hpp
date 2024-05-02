#ifndef CROSS_CORRELATION_HPP
#define CROSS_CORRELATION_HPP

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <semaphore>

// Struct to hold delay values
struct DelayValues {
    double delay_12;
    double delay_13;
    double delay_23;
};

class TrackingService{
public:
    TrackingService();
    ~TrackingService();
    DelayValues time_difference(const std::vector<double>& iq_data1, const std::vector<double>& iq_data2,
                                const std::vector<double>& iq_data3, double sample_rate, double speed_of_light);
    void cross_correlation(const std::vector<double>& iq_data1, const std::vector<double>& iq_data2, std::vector<double>& correlation, int& lag);
    double calculate_delay(const std::vector<double>& correlation, int lag, double sample_rate, double speed_of_light);
    
private:
    

};

/*
void cross_correlation(const std::vector<double>& iq_data1, const std::vector<double>& iq_data2, std::vector<double>& correlation, int& lag);
double calculate_delay(const std::vector<double>& correlation, int lag, double sample_rate, double speed_of_light);
DelayValues time_difference(const std::vector<double>& iq_data1, const std::vector<double>& iq_data2,
                            const std::vector<double>& iq_data3, double sample_rate, double speed_of_light);
//*/


#endif // CROSS_CORRELATION_HPP