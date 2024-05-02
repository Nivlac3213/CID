#include <libbladeRF.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstring>

#define SAMPLE_RATE 3000000    // Sample rate in Hz (3 MS/s)
#define BANDWIDTH 1500000      // Ensure this bandwidth is within FCC limits for your application
#define NUM_SAMPLES 16384      // Number of samples per buffer
#define FIXED_FREQUENCY 920e6  // Frequency within the 902-928 MHz band
#define TX_GAIN 60             // Adjust based on FCC power limits and your device's characteristics
#define TONE_FREQUENCY 100e3   // Tone frequency in Hz (100 kHz)

int main() {
    int status;
    struct bladerf *dev = nullptr;

    // Open device
    status = bladerf_open(&dev, nullptr);
    if (status != 0) {
        std::cerr << "Failed to open device: " << bladerf_strerror(status) << std::endl;
        return status;
    }

    // Configure device for transmission
    status = bladerf_set_sample_rate(dev, BLADERF_CHANNEL_TX(0), SAMPLE_RATE, nullptr);
    status |= bladerf_set_bandwidth(dev, BLADERF_CHANNEL_TX(0), BANDWIDTH, nullptr);
    status |= bladerf_set_frequency(dev, BLADERF_CHANNEL_TX(0), FIXED_FREQUENCY);
    status |= bladerf_set_gain(dev, BLADERF_CHANNEL_TX(0), TX_GAIN); // Review TX_GAIN for compliance

    if (status != 0) {
        std::cerr << "Failed to configure device for transmission: " << bladerf_strerror(status) << std::endl;
        bladerf_close(dev);
        return status;
    }

    // Enable the TX module
    status = bladerf_enable_module(dev, BLADERF_CHANNEL_TX(0), true);
    if (status != 0) {
        std::cerr << "Failed to enable TX module: " << bladerf_strerror(status) << std::endl;
        bladerf_close(dev);
        return status;
    }

    // Generate tone samples
    std::vector<int16_t> samples(NUM_SAMPLES * 2); // 2x for I and Q
    double samplePeriod = 1.0 / SAMPLE_RATE;
    for (size_t i = 0; i < NUM_SAMPLES; i++) {
        double phase = 2.0 * M_PI * TONE_FREQUENCY * i * samplePeriod;
        int16_t value = static_cast<int16_t>(32767 * sin(phase));
        samples[i * 2] = value;    // I component
        samples[i * 2 + 1] = value; // Q component (same for a real tone)
    }

    // Initialize sync TX configuration
    status = bladerf_sync_config(dev, BLADERF_TX_X1, BLADERF_FORMAT_SC16_Q11_META, 16, NUM_SAMPLES, 8, 10000);
    if (status != 0) {
        std::cerr << "Failed to initialize sync TX: " << bladerf_strerror(status) << std::endl;
        bladerf_close(dev);
        return status;
    }

    // Transmit samples in a loop
    struct bladerf_metadata meta;
    memset(&meta, 0, sizeof(meta));
    while (true) {
        status = bladerf_sync_tx(dev, samples.data(), NUM_SAMPLES, &meta, 10000);
        if (status != 0) {
            std::cerr << "Failed to transmit samples: " << bladerf_strerror(status) << std::endl;
            break;
        }
    }

    // Disable the TX module and close the device
    bladerf_enable_module(dev, BLADERF_CHANNEL_TX(0), false);
    bladerf_close(dev);
    return 0;
}

