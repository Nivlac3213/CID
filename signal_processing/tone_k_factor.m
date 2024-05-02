%% Used to calcualte and solve the k factor in SDR's
% Author: Calvin Henggeler

clc;
clear;
close all;

%% extract the data
% data = readmatrix("2702Tonesamples.csv", 'MissingRule','omitrow');
% save("2702Tonesamples.mat", "data");

%% Load data
data = load("2702Tonesamples.mat");
iq_data = data.data;

%% plot I and Q data
t = 1:length(iq_data);
 
% View the seperate parts
figure
subplot(2,1,1)
plot(t,iq_data(:,1))
title('I signal')

subplot(2,1,2)
plot(t, iq_data(:,2));
title('Q signal')

% Combined captured signal
y = iq_data(:,1) + iq_data(:,2);
figure
plot(t, y);

%% view signal in the frequency domain
F = fft(y);
f = linspace(902, 928, length(F));
figure
plot(f,10*log10(abs(F) / 1e-3) - 30);
ylim([-30, 60]);