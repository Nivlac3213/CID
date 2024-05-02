%% EGR310, Group 1 Project 
% December 3rd, 2023
% House Keeping 
clc
clear
close all
%% Generating a Signal filled with Unwanted Noise 
% Defining Parameters 
fo = 100e3/50e2; % Hz
fs = 1e6/50e2; % Hz
excellent_SNR = 25; % dB
good_SNR = 15; % dB
fair_SNR = 8; % dB
bad_SNR = 1; % dB
signal_duration = 1; % second
phase_shift = 0;
% Creating the Ideal RF Signal 
t = 0:1/fs:signal_duration-1/fs;
ideal_signal = cos(2*pi*fo*t - phase_shift);
% Adding Noise to the Signal, 'Adding White Guassian Noise'
real_signal = awgn(ideal_signal, good_SNR);
%% Filtering the Noise of the Signal 
% Filting the real signal to remove noise
filtered_signal = smoothdata(real_signal);
% Scaling the filtered signal to restore the amplitude 
scaling = std(real_signal)/std(filtered_signal);
% Filting the real signal to remove noise
scaled_filtered_signal = filtered_signal * scaling;
%% Interpolation of the Noisy Signal 
% Defining the amount of interpolated points
interpolation_points = 1;
t_interpolation = 0:1/(interpolation_points * fs):signal_duration;
% Interpolating with MATLAB's interp1 function 
interpolated_signal = interp1(t, scaled_filtered_signal, t_interpolation, 'makima');
%% Plotting the Results
figure(1), 
subplot(4,1,1)
plot(t, ideal_signal, 'ok', 'LineWidth', 2)
xlabel('Time (s)')
ylabel('Amplitude')
title('Ideal Radiofrequency Signal', 'FontSize', 14)
ylim([-2 2])
grid on
subplot(4,1,2)
plot(t, real_signal, 'or', 'LineWidth', 1.5)
xlabel('Time (s)')
ylabel('Amplitude')
title('Realistic Radiofrequency Signal', 'FontSize', 14)
ylim([-2 2])
grid on
subplot(4,1,3)
plot(t, scaled_filtered_signal, 'ob', 'LineWidth', 1.5)
xlabel('Time (s)')
ylabel('Amplitude')
title('Filtered Radiofrequency Signal', 'FontSize', 14)
ylim([-2 2])
grid on
subplot(4,1,4)
plot(t_interpolation, interpolated_signal, 'og', 'LineWidth', 1.5)
xlabel('Time (s)')
ylabel('Amplitude')
title('Interpolated Radiofrequency Signal', 'FontSize', 14)
ylim([-2 2])
grid on
% Taking the FFT of the Real Signal 
figure(2)
subplot(4,1,1)
plot(t, abs(fft(ideal_signal)), 'k', 'LineWidth', 2);
xlabel('Frequency (Hz)')
ylabel('Amplitude')
title('FFT of Ideal Radiofrequency Signal', 'FontSize', 14)
grid on 
subplot(4,1,2)
plot(t, abs(fft(real_signal)), 'r', 'LineWidth', 2);
xlabel('Frequency (Hz)')
ylabel('Amplitude')
title('FFT of Real Radiofrequency Signal', 'FontSize', 14)
grid on 
subplot(4,1,3)
plot(t, abs(fft(filtered_signal)), 'b', 'LineWidth', 2);
xlabel('Frequency (Hz)')
ylabel('Amplitude')
title('FFT of Filtered Radiofrequency Signal', 'FontSize', 14)
grid on 
subplot(4,1,4)
plot(t_interpolation, abs(fft(interpolated_signal)), 'g', 'LineWidth', 2);
xlabel('Frequency (Hz)')
ylabel('Amplitude')
title('FFT of Interpolated Radiofrequency Signal', 'FontSize', 14)
grid on 
% Interpolate NaN values in interpolated_signal
interpolated_signal = fillmissing(interpolated_signal, 'makima');
real_signal = fillmissing(real_signal, 'makima');
scaled_filtered_signal = fillmissing(scaled_filtered_signal, 'makima');
% Remove infinite values
interpolated_signal = interpolated_signal(isfinite(interpolated_signal));
scaled = real_signal(isfinite(real_signal));
scaled_filtered_signal = scaled_filtered_signal(isfinite(scaled_filtered_signal));
%% Calculate the power spectral density using pwelch
[psd, frequencies] = pwelch(interpolated_signal, [], [], [], fs);
[psd_real, frequencies_real] = pwelch(real_signal, [], [], [], fs);
[psd_filtered, frequencies_filtered] = pwelch(scaled_filtered_signal, [], [], [], fs);
%% Plot the power spectral density
figure(3)
subplot(3,1,1)
plot(frequencies, 10*log10(psd), 'b', 'LineWidth', 1.5);
xlabel('Frequency (Hz)');
ylabel('Power/Frequency (dB/Hz)');
title('Power Spectral Density of Interpolated Signal', 'FontSize', 14);
grid on;
subplot(3,1,2)
plot(frequencies_real, 10*log10(psd_real), 'b', 'LineWidth', 1.5);
xlabel('Frequency (Hz)');
ylabel('Power/Frequency (dB/Hz)');
title('Power Spectral Density of real Signal', 'FontSize', 14);
grid on;
subplot(3,1,3)
plot(frequencies_filtered, 10*log10(psd_filtered), 'b', 'LineWidth', 1.5);
xlabel('Frequency (Hz)');
ylabel('Power/Frequency (dB/Hz)');
title('Power Spectral Density of filtered Signal', 'FontSize', 14);
grid on;
%% Integrate the PSD to get total power
total_power_dB = 10*log10(trapz(frequencies, 10.^(psd/10)));
total_power_dB_real_signal = 10*log10(trapz(frequencies_real, 10.^(psd_real/10)));
total_power_dB_filtered_signal = 10*log10(trapz(frequencies_filtered, 10.^(psd_filtered/10)));
% cumulative power across all frequency components in the signal.
fprintf('Total Power (dB): %f dB\n', total_power_dB);
fprintf('Total Power in real signal (dB): %f dB\n', total_power_dB_real_signal);
fprintf('Total Power in filtered signal (dB): %f dB\n\n', total_power_dB_filtered_signal);
% Plot the power in dB vs frequency for the interpolated signal
figure(4)
subplot(3,1,1)
plot(frequencies, 10*log10(psd*fs), 'b', 'LineWidth', 1.5);
xlabel('Frequency (Hz)');
ylabel('Power (dB)');
title('Power vs Frequency of Interpolated Signal', 'FontSize', 14);
grid on;
subplot(3,1,2)
plot(frequencies_real, 10*log10(psd_real*fs), 'b', 'LineWidth', 1.5);
xlabel('Frequency (Hz)');
ylabel('Power (dB)');
title('Power vs Frequency of real Signal', 'FontSize', 14);
grid on;
subplot(3,1,3)
plot(frequencies_filtered, 10*log10(psd_filtered*fs), 'b', 'LineWidth', 1.5);
xlabel('Frequency (Hz)');
ylabel('Power (dB)');
title('Power vs Frequency of filtered Signal', 'FontSize', 14);
grid on;
[max_psd, max_psd_index] = max(10*log10(psd));
fprintf('Maximum Power Spectral Density: %f dB/Hz\n', max_psd);
fprintf('Frequency at Maximum Power Spectral Density: %f Hz\n\n', frequencies(max_psd_index));
[max_psd_real_signal, max_psd_index_real_signal] = max(10*log10(psd_real));
fprintf('Maximum Power Spectral Density of real signal: %f dB/Hz\n', max_psd_real_signal);
fprintf('Frequency at Maximum Power Spectral Density of real signal: %f Hz\n\n', ...
 frequencies(max_psd_index_real_signal));
[max_psd_filtered_signal, max_psd_index_filtered_signal] = max(10*log10(psd_filtered));
fprintf('Maximum Power Spectral Density of filtered signal: %f dB/Hz\n', max_psd_filtered_signal);
fprintf('Frequency at Maximum Power Spectral Density of filtered signal: %f Hz\n\n', frequencies(max_psd_index_filtered_signal));
% Find the minimum and maximum amplitudes
min_value = min(interpolated_signal);
max_value = max(interpolated_signal);
% Find the minimum and maximum power
min_power_interpolated = min(10*log10(psd*fs));
max_power_interpolated = max(10*log10(psd*fs));
min_power_real = min(10*log10(psd_real*fs));
max_power_real = max(10*log10(psd_real*fs));
min_power_filtered = min(10*log10(psd_filtered*fs));
max_power_filtered = max(10*log10(psd_filtered*fs));
% Find the time corresponding to the minimum and maximum amplitudes
time_at_min = t_interpolation(find(interpolated_signal == min_value, 1));
time_at_max = t_interpolation(find(interpolated_signal == max_value, 1));
% Frequency at which maximum and minimum power occur for interpolated signal in dB
freq_at_max_power_interpolated = frequencies(max_psd_index);
freq_at_max_power_real_signal = frequencies_real(max_psd_index_real_signal);
freq_at_min_power_filtered_signal = frequencies_filtered(max_psd_index_filtered_signal);
% Display the results
fprintf('Minimum Value %f at time %f \n', min_value, time_at_min);
fprintf('Maximum Value: %f at time %f \n\n', max_value, time_at_max);
% Display Max power at the according frequency
fprintf('Maximum power of interpolated signal: %f at %f Hz \n\n', ...
 max_power_interpolated,freq_at_max_power_interpolated);
fprintf('Maximum power of real signal: %f at %f Hz\n\n', ...
 max_power_real,freq_at_max_power_real_signal);
fprintf('Maximum power of filtered signal: %f at %f Hz \n', ...
 max_power_filtered,freq_at_min_power_filtered_signal);
% phase shift calculations
%ideal_signal
x1 = 0; x2 = signal_duration-1/fs;
f = @(x) cos(2*pi*fo*x - phase_shift);
x = 0:0.1/fs:signal_duration-1/fs;
N = length(x)-1;
I_trp = 0;
for i=1:N
 xi = x(i); xi1=x(i+1);
 h2 = 0.5*(xi1-x1);
 I_trp = I_trp + h2*f(f(xi)+f(xi1));
end
figure(5); 
plot(x * (180/pi),f(x), 'r', 'LineWidth', 2)
hold on
zero_phase = I_trp;
% find phase
I_trp = 0;
phase_shift = 0;
f =@(x) cos(2*pi*fo*x - phase_shift); %signal function
for i=1:N
 xi = x(i); xi1=x(i+1);
 h2 = 0.5*(xi1-x1);
 I_trp = I_trp + h2*f(f(xi)+f(xi1));
end
phase = zero_phase - I_trp;
%% Plotting the Results
plot(x * (180/pi),f(x), 'b', 'LineWidth', 2)
legend('Foundational Phase', 'Shifted Phase')
xlabel('Time (s)')
ylabel('Amplitude')
title('Phase Shifted Ideal RF Signal', 'FontSize', 14)
ylim([-2 2])
grid on
xlim([0,10]);
fprintf("\n");
fprintf("Phase Shift in degrees: %f\n", phase);
