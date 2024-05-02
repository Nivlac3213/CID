#ifndef RXW_FUNCTIONS_HPP
#define RXW_FUNCTIONS_HPP

// #include <libbladeRF.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>

#define FREQUENCY 925e6  // Frequency in Hz
#define SAMPLE_RATE 2e6  // Sample rate in Hz
#define NUM_SAMPLES 16000 // Number of samples
#define MAX_ADC_VALUE 2047.0 // Maximum value for 12-bit ADC
#define FULL_SCALE_VOLTAGE 0.625 // Full scale voltage in V peak for ADC
#define LOAD_IMPEDANCE 50.0 // Load impedance in Ohms
#define NOISE_FLOOR -5 // Noise Floor at -8.5 dBm
#define THRESHOLD -4 //-3 // Threshold 15 dBm above noise floor

double processSamplePower(const std::vector<int16_t>& samples);
double processSamplePower(int16_t samples[]);
void printThresholdDetection(double totalPowerdBm);
bool thresholdDetection(double totalPowerdBm);
std::vector<int16_t> vectorizeSamples(int16_t samples[]);

#endif // RXW_FUNCTIONS_HPP