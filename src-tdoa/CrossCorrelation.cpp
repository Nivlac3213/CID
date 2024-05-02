// Author:   Zachary Traynor + Zachary Hart
// Date:     04/17/24
// Revision: 1 - Implementation of Cross-Correlation and Time Delay Logic 
//
// File Name: Cross Correlation
// Inputs:    I/Q Vectors 
// Outputs:   Delta T

// Header Files
#include "CrossCorrelation.hpp"
#include <iostream>

// Struct to hold delay values
//struct DelayValues {
//    double delay_12;
//    double delay_13;
//    double delay_23;
//};

// Function Name:
//  cross_correlation
//
// Description: 
//  Performs cross-correlation between two arrays 
//
// Inputs:
//  iq_data1    - array of IQ data
//  iq_data2    - array of IQ data
//  correlation - output vector to store cross-correlation result 
//  lag         - reference to an integeter to store the lag value 
//
// Outputs:
//  correlation - cross-correlation result
//  lag         - lag value indicating time shift
void cross_correlation(const std::vector<short int>& iq_data1, const std::vector<short int>& iq_data2, std::vector<double>& correlation, int& lag) {
    
    // Defining Sizes 
    int size1 = iq_data1.size();   
    int size2 = iq_data2.size();   
    correlation.resize(size1 + size2 - 1, 0);

    // Compute Cross-Correlation
    for (int i = 0; i < size1; ++i) {
        for (int j = 0; j < size2; ++j) {
            correlation[i + j] += iq_data1[i] * iq_data2[j];
        }
    }

    // Find the Index of the Maximum Correlation
    auto max_corr_iter = std::max_element(correlation.begin(), correlation.end());
    lag = std::distance(correlation.begin(), max_corr_iter) - (size1 - 1);
}

// Function Name:
//  calculate_delay_and_distance
//
// Description: 
//  Calculates delay and distance based on correlation, sample rate, and speed of light
//
// Inputs:
//  correlation    - cross-correlation result
//  lag            - lag value indicating time shift
//  sample_rate    - sample rate
//  speed_of_light - speed of light
//
// Outputs:
//  delay - time difference between two modules 
double calculate_delay(const std::vector<double>& correlation, int lag, double sample_rate, double speed_of_light) {
    
    // Calculate the time shift
    double lag_time = lag / sample_rate;

    // Define delay
    double delay = lag_time;

    // Output Delay
    return delay;
}

// Function Name:
//  time_difference
//
// Description: 
//  
// Inputs:
//
// Outputs:
//  
DelayValues time_difference(const std::vector<short int>& iq_data1, const std::vector<short int>& iq_data2,
                            const std::vector<short int>& iq_data3, double sample_rate, double speed_of_light){

    // Perform Cross-Correlation Between Rx1 and Rx2
    std::vector<double> correlation1;
    int lag1;
    cross_correlation(iq_data1, iq_data2, correlation1, lag1);
    double delay_12 = calculate_delay(correlation1, lag1, sample_rate, speed_of_light);

    // Perform Cross-Correlation Between Rx1 and Rx3
    std::vector<double> correlation2;
    int lag2;
    cross_correlation(iq_data1, iq_data3, correlation2, lag2);
    double delay_13 = calculate_delay(correlation2, lag2, sample_rate, speed_of_light);

    // Perform Cross-Correlation Between Rx2 and Rx3
    std::vector<double> correlation3;
    int lag3;
    cross_correlation(iq_data2, iq_data3, correlation3, lag3);
    double delay_23 = calculate_delay(correlation3, lag3, sample_rate, speed_of_light);

    return {delay_12, delay_13, delay_23};

}

//int main() {
//
//
//    std::vector<double> iq_data1, iq_data2, iq_data3;
//
//    // Defining Constants
//    double sample_rate    = 2000000;
//    double speed_of_light = 299792458;
//
//    // Call Time-Difference Function
//    DelayValues delays = time_difference(iq_data1, iq_data2, iq_data3, sample_rate, speed_of_light);
//
//    return 0;
//}

// ORIGINAL MAIN
/*int main() {

    // Pointing  I/Q Vectors
    std::vector<double>* iq_data1;
    std::vector<double>* iq_data2;
    std::vector<double>* iq_data3;

    // Defining Constants
    double sample_rate    = 2000000;
    double speed_of_light = 299792458;

    // Perform Cross-Correlation Between Rx1 and Rx2
    std::vector<double> correlation1;
    int lag1;
    cross_correlation(iq_data1, iq_data2, correlation1, lag1);
    double delay_12 = calculate_delay(correlation1, lag1, sample_rate, speed_of_light);

    // Perform Cross-Correlation Between Rx1 and Rx3
    std::vector<double> correlation2;
    int lag2;
    cross_correlation(iq_data1, iq_data3, correlation2, lag2);
    double delay_23 = calculate_delay(correlation2, lag2, sample_rate, speed_of_light);

    return 0;
}*/
