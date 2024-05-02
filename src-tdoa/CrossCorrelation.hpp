// Author:   Zachary Traynor + Zachary Hart
// Date:     04/17/24
// Revision: 1 - Implementation of Cross-Correlation and Time Delay Logic
//
// Header File for Cross Correlation
// Inputs:    I/Q Vectors
// Outputs:   Delta T

#ifndef CROSS_CORRELATION_HPP
#define CROSS_CORRELATION_HPP

#include <vector>
#include <algorithm> // for std::max_element and std::distance

// Struct to hold delay values
struct DelayValues {
    double delay_12;
    double delay_13;
    double delay_23;
};

// Function declarations
void cross_correlation(const std::vector<double>& iq_data1, const std::vector<double>& iq_data2,
                       std::vector<double>& correlation, int& lag);

double calculate_delay(const std::vector<double>& correlation, int lag,
                       double sample_rate, double speed_of_light);

DelayValues time_difference(const std::vector<short int>& iq_data1, const std::vector<short int>& iq_data2,
                            const std::vector<short int>& iq_data3, double sample_rate, double speed_of_light);

#endif // CROSS_CORRELATION_HPP
