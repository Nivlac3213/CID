#include <libbladeRF.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>

#define SAMPLE_RATE 3000000    // Sample rate in Hz (3 MS/s)
#define BANDWIDTH 5000000      // Bandwidth in Hz
#define NUM_SAMPLES 16384      // Number of samples to capture
#define TIMEOUT_MS 1000        // Timeout for sync RX in milliseconds
#define FIXED_FREQUENCY 1000e6 // Fixed frequency at 1 GHz
#define CSV_FILE "iq_data.csv" // CSV file name
#define POWER_THRESHOLD -57    // Power threshold in dBm, TO BE DETERMINED
#define FIXED_GAIN 0           // Fixed gain in dB



int main() {
    int status;
    struct bladerf *dev = nullptr;

    // Open device
    status = bladerf_open(&dev, nullptr);
    if (status != 0) {
        std::cerr << "Failed to open device: " << bladerf_strerror(status) << std::endl;
        return status;
    }

    // Configure global settings
    status = bladerf_set_sample_rate(dev, BLADERF_CHANNEL_RX(0), SAMPLE_RATE, nullptr);
    status |= bladerf_set_bandwidth(dev, BLADERF_CHANNEL_RX(0), BANDWIDTH, nullptr);
    status |= bladerf_set_frequency(dev, BLADERF_CHANNEL_RX(0), FIXED_FREQUENCY);
    if (status != 0) {
        std::cerr << "Failed to configure device: " << bladerf_strerror(status) << std::endl;
        bladerf_close(dev);
        return status;
    }

    // Set fixed gain value

    status = bladerf_set_gain(dev, BLADERF_CHANNEL_RX(0), FIXED_GAIN);
    if (status != 0) {
        std::cerr << "Failed to set fixed gain: " << bladerf_strerror(status) << std::endl;
        bladerf_close(dev);
        return status;
    }

    // Allocate buffer for samples
    std::vector<int16_t> samples(NUM_SAMPLES * 2); // 2x for I and Q
    struct bladerf_metadata meta;

    // Initialize sync RX configuration
    status = bladerf_sync_config(dev, BLADERF_RX_X1, BLADERF_FORMAT_SC16_Q11_META, 16, NUM_SAMPLES, 8, TIMEOUT_MS);
    if (status != 0) {
        std::cerr << "Failed to initialize sync RX: " << bladerf_strerror(status) << std::endl;
        bladerf_close(dev);
        return status;
    }

    // Enable the RX module
    status = bladerf_enable_module(dev, BLADERF_CHANNEL_RX(0), true);
    if (status != 0) {
        std::cerr << "Failed to enable RX module: " << bladerf_strerror(status) << std::endl;
        bladerf_close(dev);
        return status;
    }

    // Open CSV file for writing
    std::ofstream csvFile(CSV_FILE);
    if (!csvFile.is_open()) {
        std::cerr << "Failed to open CSV file for writing." << std::endl;
        bladerf_close(dev);
        return -1;
    }

    // Write header to CSV file
    csvFile << "I,Q,Power\n";

    // Infinite loop for continuous operation
    // NOTE: To quit, ctrl + C. No exit condition implemented 
    while (true) {
        // Prepare for capture without specifying a future timestamp
        memset(&meta, 0, sizeof(meta));
        meta.flags = BLADERF_META_FLAG_RX_NOW;

        // Capture samples
        status = bladerf_sync_rx(dev, samples.data(), NUM_SAMPLES, &meta, TIMEOUT_MS);
        if (status != 0) {
            std::cerr << "Failed to receive samples: " << bladerf_strerror(status) << std::endl;
            continue; // Continue trying to capture samples
        }

        // Process samples and write to CSV
        // TODO: Need to find hardware scaling constant
        for (size_t i = 0; i < NUM_SAMPLES; i++) {
            int16_t I = samples[i * 2];     // I component
            int16_t Q = samples[i * 2 + 1]; // Q component
            double power = (I * I + Q * Q);   // power in W or mW?
            double powerdBm = 10 * log10(power/1e-3); // Instantaneous power in dBm

            if (powerdBm > POWER_THRESHOLD) {
                csvFile << I << "," << Q << "," << powerdBm << "\n";
            }
        }
    }

    // The program will never reach this point due to the infinite loop
    // NOTE: Close the device and the CSV file after exit condition
    // csvFile.close();
    // bladerf_close(dev);
    return 0;
}

