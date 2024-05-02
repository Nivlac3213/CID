// #include <libbladeRF.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include "RxwFunctions.hpp"


std::vector<int16_t> vectorizeSamples(int16_t samples[]){
    std::vector<int16_t> samplesVector;
    for (size_t i = 0; i < NUM_SAMPLES; ++i) {
        samplesVector.push_back(samples[i]);
    }
    return samplesVector;
}

//*
double processSamplePower(const std::vector<int16_t>& samples){
    double totalPowerdBm = 0;
    //std::cout << "Size of array: " << samples.size() << std::endl;
    for (size_t i = 0; i < NUM_SAMPLES/2; ++i) {
        if (2*i >= samples.size()) break; // Ensure we do not over index
        double I_voltage = (samples[2 * i] / MAX_ADC_VALUE) * FULL_SCALE_VOLTAGE / 1.414213; // Convert to Vrms
        double Q_voltage = (samples[2 * i + 1] / MAX_ADC_VALUE) * FULL_SCALE_VOLTAGE / 1.414213; // Convert to Vrms

        double powerWatts = (pow(I_voltage, 2) + pow(Q_voltage, 2)) / LOAD_IMPEDANCE;
        double powerdBm = 10 * log10(powerWatts) + 30; // Convert to dBm

        //std::cout << "Power of sample " << i << " = " << powerdBm << " dBm" << std::endl;
        totalPowerdBm+=powerdBm;
    } 
    return totalPowerdBm;
}
//*/

//*
double processSamplePower(int16_t samples[]) {
    double totalPowerdBm = 0;

    // Ensure we have an even number of samples for I/Q pairs
    // size_t num_pairs = NUM_SAMPLES / 2;

    for (size_t i = 0; i < NUM_SAMPLES; ++i) {
        double I_voltage = (samples[2 * i] / MAX_ADC_VALUE) * FULL_SCALE_VOLTAGE / 1.414213; // Convert to Vrms
        double Q_voltage = (samples[2 * i + 1] / MAX_ADC_VALUE) * FULL_SCALE_VOLTAGE / 1.414213; // Convert to Vrms

        double powerWatts = (pow(I_voltage, 2) + pow(Q_voltage, 2)) / LOAD_IMPEDANCE;
        double powerdBm = 10 * log10(powerWatts) + 30; // Convert to dBm

        totalPowerdBm += powerdBm;
    } 
    return totalPowerdBm;
}
//*/

void printThresholdDetection(double totalPowerdBm){
    double avgPowerdBm = totalPowerdBm/NUM_SAMPLES;
    //std::cout << "Average Power: " << avgPowerdBm << " dBm" << std::endl;
    if(avgPowerdBm > NOISE_FLOOR + THRESHOLD){
        std::cout << "Signal Detected Above Threshold" << std::endl;
    }
}

bool thresholdDetection(double totalPowerdBm){
    // double avgPowerdBm = totalPowerdBm/NUM_SAMPLES;
    if(totalPowerdBm > THRESHOLD){
        return true;
        std::cout << "Signal Detected: " << std::endl;  
    }
    return false;
}


/*
int main() {
    int status;
    struct bladerf *dev = nullptr;
    unsigned int start_freq = 600e6, end_freq = 1000e6, step_freq = 100e6;
    // Open the device
    status = bladerf_open(&dev, nullptr);
    if (status != 0) {
        std::cerr << "Failed to open device: " << bladerf_strerror(status) << std::endl;
        return -1;
    }

    status = bladerf_set_gain_mode(dev, BLADERF_CHANNEL_RX(0), BLADERF_GAIN_MANUAL);
    if (status != 0) {
        std::cerr << "Failed to disable AGC: " << bladerf_strerror(status) << std::endl;
        // Handle error (e.g., cleanup and exit)
    }

    
    if (status != 0) {
        std::cerr << "Failed to configure sync: " << bladerf_strerror(status) << std::endl;
        bladerf_enable_module(dev, BLADERF_CHANNEL_RX(0), false);
        bladerf_close(dev);
        return -1;
    }

    for (unsigned int frequency = start_freq; frequency <= end_freq; frequency += step_freq) {
        std::cout << "Monitoring Frequency: " << frequency << std::endl;
        // Set frequency and sample rate
        status = bladerf_set_frequency(dev, BLADERF_CHANNEL_RX(0), frequency);
        status |= bladerf_set_sample_rate(dev, BLADERF_CHANNEL_RX(0), SAMPLE_RATE, nullptr);
        if (status != 0) {
            std::cerr << "Failed to configure device: " << bladerf_strerror(status) << std::endl;
            bladerf_close(dev);
            return -1;
        }
        // Enable RX module
        status = bladerf_enable_module(dev, BLADERF_CHANNEL_RX(0), true);
        if (status != 0) {
            std::cerr << "Failed to enable RX module: " << bladerf_strerror(status) << std::endl;
            bladerf_close(dev);
            return -1;
        }

        
        // Configure synchronization
        status = bladerf_sync_config(dev,
                                     BLADERF_RX_X1,
                                     BLADERF_FORMAT_SC16_Q11,
                                     32,
                                     32768,
                                     16,
                                     1000);

        std::vector<int16_t> samples(NUM_SAMPLES * 2); // 2x for I and Q components

        double totalPowerdBm = 0;
                // Receive a batch of samples
        status = bladerf_sync_rx(dev, samples.data(), NUM_SAMPLES, nullptr, 10000);
        if (status != 0) {
            std::cerr << "Failed to receive samples: " << bladerf_strerror(status) << std::endl;
        } else {    
            totalPowerdBm = processSamples(samples);
        }

        thresholdDetection(totalPowerdBm);
        Sleep(1000);
    }

    // Cleanup
    bladerf_enable_module(dev, BLADERF_CHANNEL_RX(0), false);
    bladerf_close(dev);

    return 0;
}
//*/