#include <libbladeRF.h>
#include <iostream>
#include <vector>
#include <cstring>
#include <iomanip> // For std::setprecision
#include <fstream> // For file operations

#define SAMPLE_RATE 2000000  // Sample rate in Hz (2 MS/s)
#define BANDWIDTH 1500000    // Bandwidth in Hz
#define NUM_SAMPLES 16384    // Number of samples to capture at each step
#define TIMEOUT_MS 1000      // Timeout for sync RX in milliseconds

int main() {
    int status;
    struct bladerf *dev = nullptr;
    struct bladerf_metadata meta;
    unsigned int start_freq = 700e6, end_freq = 1900e6, step_freq = 5e6;

    // Open device
    status = bladerf_open(&dev, nullptr);
    if (status != 0) {
        std::cerr << "Failed to open device: " << bladerf_strerror(status) << std::endl;
        return status;
    }

    // Configure global settings
    status = bladerf_set_sample_rate(dev, BLADERF_CHANNEL_RX(0), SAMPLE_RATE, nullptr);
    status |= bladerf_set_bandwidth(dev, BLADERF_CHANNEL_RX(0), BANDWIDTH, nullptr);
    if (status != 0) {
        std::cerr << "Failed to configure device: " << bladerf_strerror(status) << std::endl;
        bladerf_close(dev);
        return status;
    }

    // Allocate buffer for samples
    std::vector<int16_t> samples(NUM_SAMPLES * 2); // 2x for I and Q

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

    // Open CSV file
    std::ofstream csvFile;
    csvFile.open("captured_data.csv");
    // Write header
    csvFile << "Frequency (Hz),FPGA Timestamp,Real Time (s),Sample Index,I Sample,Q Sample\n";

    // Frequency sweep
    for (unsigned int frequency = start_freq; frequency <= end_freq; frequency += step_freq) {
        // Set frequency
        status = bladerf_set_frequency(dev, BLADERF_CHANNEL_RX(0), frequency);
        if (status != 0) {
            std::cerr << "Failed to set frequency to " << frequency << " Hz: " << bladerf_strerror(status) << std::endl;
            continue;  // Skip this step if frequency set fails
        }

        // Prepare for capture without specifying a future timestamp
        memset(&meta, 0, sizeof(meta));
        meta.flags = BLADERF_META_FLAG_RX_NOW;

        // Capture samples
        status = bladerf_sync_rx(dev, samples.data(), NUM_SAMPLES, &meta, TIMEOUT_MS);
        if (status != 0) {
            std::cerr << "Failed to receive samples: " << bladerf_strerror(status) << std::endl;
        } else {
            // Here, meta.timestamp will be filled with the FPGA's sample count timestamp of the first sample

            // Convert FPGA timestamp to real-world time (seconds)
            double timestampSeconds = meta.timestamp / static_cast<double>(SAMPLE_RATE);
            std::cout << "Captured " << NUM_SAMPLES << " samples at " << frequency << " Hz. ";
            std::cout << "FPGA timestamp: " << meta.timestamp << " (approx. " << timestampSeconds << " seconds since start of capture)" << std::endl;

            // Write captured data to CSV file
            for (size_t i = 0; i < NUM_SAMPLES; i++) {
                csvFile << frequency << "," << meta.timestamp << "," << std::setprecision(10) << timestampSeconds << ",";
                csvFile << i << "," << samples[i * 2] << "," << samples[i * 2 + 1] << "\n";
            }
        }
    }

    // Disable the RX module before cleanup
    bladerf_enable_module(dev, BLADERF_CHANNEL_RX(0), false);

    // Cleanup
    bladerf_close(dev);

    // Close CSV file
    csvFile.close();

    return 0;
}
