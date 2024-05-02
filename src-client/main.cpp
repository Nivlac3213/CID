/* 
 * This runs the program that will control the SDR to capture and packetize
 * signal data. This program is to be run in a seperate thread on the RPi-4
 * Save to a file, e.g. boilerplate.c, and then compile:
 * $ g++ main.cpp SignalDataService.cpp -o clientApp -lbladeRF -lpthread
 */
 
#include <libbladeRF.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include "SignalDataService.hpp"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <chrono>

#define MODE_DEV               // Preprocessor Mode for development and testing
#define CSV_FILE "iq_data.csv" // CSV file name

#define SAMPLE_RATE 10000000   // Sample rate in Hz (10 MS/s)
#define BANDWIDTH 5000000      // Bandwidth in Hz
#define NUM_SAMPLES 16384      // Number of samples to capture
#define TIMEOUT_MS 1000        // Timeout for sync RX in milliseconds
#define FIXED_FREQUENCY 920e6  // Fixed frequency at 100 MHx
#define POWER_THRESHOLD -57    // Power threshold in dBm, TO BE DETERMINED
#define FIXED_GAIN 0           // Fixed gain in dB

/* The RX and TX channels are configured independently for these parameters */
struct channel_config {
 bladerf_channel channel;
 unsigned int frequency;
 unsigned int bandwidth;
 unsigned int samplerate;
 int gain;
};

/* uses at channel_config struct to configure a cahnnel on BladeRf SDR*/
int configure_channel(struct bladerf *dev, struct channel_config *c)
{
    int status;
    
    // Set the center frequency
    status = bladerf_set_frequency(dev, c->channel, c->frequency);
    if (status != 0) {
    fprintf(stderr, "Failed to set frequency = %u: %s\n", c->frequency,
    bladerf_strerror(status));
    return status;
    }
    
    // Set the sampling rate
    status = bladerf_set_sample_rate(dev, c->channel, c->samplerate, NULL);
    if (status != 0) {
    fprintf(stderr, "Failed to set samplerate = %u: %s\n", c->samplerate,
    bladerf_strerror(status));
    return status;
    }
    
    // Set the cahnnel bandwidth
    status = bladerf_set_bandwidth(dev, c->channel, c->bandwidth, NULL);
    if (status != 0) {
    fprintf(stderr, "Failed to set bandwidth = %u: %s\n", c->bandwidth,
    bladerf_strerror(status));
    return status;
    }
    
    // Set the gain
    status = bladerf_set_gain(dev, c->channel, c->gain);
    if (status != 0) {
    fprintf(stderr, "Failed to set gain: %s\n", bladerf_strerror(status));
    return status;
    }
    
    return status;
}

// retrieve the synchronized time in nanoseconds since the epoch
uint64_t getCurrentSystemTime() {
    auto now = std::chrono::system_clock::now(); // get the current time point
    auto duration_since_epoch = now.time_since_epoch(); // get the duration since epoch

    // convert duration to nanoseconds
    uint64_t nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration_since_epoch).count();

    return nanoseconds;
}
 
/* Usage:
 * libbladeRF_example_boilerplate [serial #]
 *
 * If a serial number is supplied, the program will attempt to open the
 * device with the provided serial number.
 *
 * Otherwise, the first available device will be used.
 */
int main(int argc, char *argv[])
{
    int status;
    struct channel_config config;
    
    struct bladerf *dev = NULL;
    struct bladerf_devinfo dev_info;
    
    /* Initialize the information used to identify the desired device
    * to all wildcard (i.e., "any device") values */
    bladerf_init_devinfo(&dev_info);
    
    /* Request a device with the provided serial number.
    * Invalid strings should simply fail to match a device. */
    if (argc >= 2) {
    strncpy(dev_info.serial, argv[1], sizeof(dev_info.serial) - 1);
    }
    
    status = bladerf_open_with_devinfo(&dev, &dev_info);
    if (status != 0) {
       fprintf(stderr, "Unable to open device: %s\n",
       bladerf_strerror(status));
       return 1;
    }
    
    // Set up RX channel parameters
    config.channel      = BLADERF_CHANNEL_RX(0);
    config.frequency    = FIXED_FREQUENCY;
    config.bandwidth    = BANDWIDTH;
    config.samplerate   = SAMPLE_RATE;
    config.gain         = FIXED_GAIN;
 
    status = configure_channel(dev, &config);
    if (status != 0) {
        fprintf(stderr, "Failed to configure RX channel. Exiting.\n");
        //goto out;
        return -1;
    }

    // Initialize sync RX configuration for timestampled RX
    status = bladerf_sync_config(dev, BLADERF_RX_X1, BLADERF_FORMAT_SC16_Q11_META, 16, NUM_SAMPLES, 8, TIMEOUT_MS);
    if (status != 0) {
        std::cerr << "Failed to initialize sync RX: " << bladerf_strerror(status) << std::endl;
        bladerf_close(dev);
        return status;
    }

    // Enable the RX module (Should be the last call before App Code)
    status = bladerf_enable_module(dev, BLADERF_CHANNEL_RX(0), true);
    if (status != 0) {
        std::cerr << "Failed to enable RX module: " << bladerf_strerror(status) << std::endl;
        bladerf_close(dev);
        return status;
    }
    
    //
    // ===================== socket instantiation begin =====================
    //

    int sock = 0;
    struct sockaddr_in serv_addr;

    // create socket file descriptor
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);

    // convert IP addy from text to binary form
    if(inet_pton(AF_INET, "172.19.172.40", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        return -1;
    }

    // attempt server connection
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection Failed" << std::endl;
        return -1;
    }

    std::cout << "Connected to the server successfully." << std::endl;

    //
    // ===================== socket instantiation end =====================
    //

    // Allocate buffer for samples
    std::vector<int16_t> samples(NUM_SAMPLES * 2); // 2x for I and Q
    struct bladerf_metadata meta;

 
    #ifdef MODE_DEV /* File for writing temp data*/
    /*
    // Open CSV file for writing
    std::ofstream csvFile(CSV_FILE);
    if (!csvFile.is_open()) {
        std::cerr << "Failed to open CSV file for writing." << std::endl;
        bladerf_close(dev);
        return -1;
    }

    // Write header to CSV file
    csvFile << "I_avg,Q_avg,timestamp\n";
    //*/
    #endif /* MODE_DEV */

    /* ---------- WHILE LOOP ---------- */
    // NOTE: To quit, ctrl + C. No exit condition implemented 
    while (true) {

        // synchronized system clock time (assuming the ptp4l system service is running)
        // to check if the service is running, execute 'sudo systemctl status ptp4l.service'
        uint64_t current_time_ns = getCurrentSystemTime();
        // TODO Calvin plz send this value to the SDR to update it's clock thank you :)

        // Prepare for capture without specifying a future timestamp
        memset(&meta, 0, sizeof(meta));
        meta.flags = BLADERF_META_FLAG_RX_NOW;

        // Capture samples
        status = bladerf_sync_rx(dev, samples.data(), NUM_SAMPLES, &meta, TIMEOUT_MS);
        if (status != 0) {
            std::cerr << "Failed to receive samples: " << bladerf_strerror(status) << std::endl;
            continue; // Continue trying to capture samples
        }

        #ifdef  MODE_DEV /* print filled buffer length and time stamp*/
        std::cout << "FPGA Time: " << meta.timestamp << " Sample Len: " << meta.actual_count << std::endl;
        #endif           /* MODE_DEV */
        
        // it a buffer to hold exactly 16384 samples
        // ensures buffers sent are always the same length
        std::vector<int16_t> fixedSamples(FIXED_SAMPLE_COUNT, 0);  // init to zero
        std::copy(samples.begin(), samples.begin() + std::min(static_cast<unsigned int>(FIXED_SAMPLE_COUNT), meta.actual_count), fixedSamples.begin());

        time_series_req_t req = {meta.timestamp, int16_t(fixedSamples.size()), fixedSamples.data()};

        SignalDataService service;
        buffer_t serialized = service.time_series_serialize_req(req);
        send(sock, serialized.data, serialized.length, 0);
        std::cout << "Time series data sent successfully." << std::endl;

        // receive response
        char buffer[1024] = {0};
        int valread = read(sock, buffer, 1024);
        if (valread > 0) {
            time_series_resp_t response;
            buffer_t responseBuffer = {buffer, static_cast<size_t>(valread)};
            service.time_series_deserialize_resp(responseBuffer, &response);
            std::cout << "Message received from server: id=" << response.id << ", status=" << response.status << std::endl;
        }


    }/* ---------- END LOOP ---------- */

    // ===================== End Application Code =====================

} /* End Main() */


/*
out:
    bladerf_close(dev);
    return status;
}
//*/