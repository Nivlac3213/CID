close all;

% Cleanup
% clear all;
% close all;
% % ------------------------------------------------------------------------
% Initialization
D = 10; % dimensions of a room
nRx = 3; % Number of receivers
nTx = 1; % Number of emitters
dim = 2; % Number of dimensions for the problem
% Rx = D*rand(nRx, dim); % Rx locations x, y in meters
% Rx = abs(Rx); % only pos coordinates
Tx = D*rand(nTx, dim); % Tx location x, y in meters
Tx = abs(Tx); % only pos coordinates
c = 299792458; % speed of light in m/s
Sample_Rate = 1400000000;
% 
% Rx receiver positions
Rx = [0,0;D,0;.25*D,D];
real = Tx;
calculated = zeros(1,2);

% % ------------------------------------------------------------------------
% TDOA
% % ------------------------------------------------------------------------

for i=1:1:14 % Simulation steps
    % i = 1;
    if (D-Tx(1)>Tx(1)-0)
        Tx(1)=Tx(1)+0.2;
    end
    if (D-Tx(1)<Tx(1)-0)
        Tx(1)=Tx(1)-0.2;
    end
    if (D-Tx(2)>Tx(2)-0)
        Tx(2)=Tx(2)+0.2;
    end
    if (D-Tx(2)<Tx(2)-0)
        Tx(2)=Tx(2)-0.2;
    end
    real(i,:) = Tx;
    
% time inputs
% time1; time2; time3;
% distance1 = time1*c; distance2 = time2*c; distance3 = time3*c;

% Time = Distance / c
% Distance formula sqrt((x1-x)^2 + (y1-y)^2)
distance1 = sqrt((Rx(1,1) - Tx(1))^2 + (Rx(1,2) - Tx(2))^2); % example distance
% time1 = distance1/c;
% Distance formula sqrt((x2-x)^2 + (y2-y)^2)
distance2 = sqrt((Rx(2,1) - Tx(1))^2 + (Rx(2,2) - Tx(2))^2); % example distance
% time2 = distance2/c;
% Distance formula sqrt((x3-x)^2 + (y3-y)^2)
distance3 = sqrt((Rx(3,1) - Tx(1))^2 + (Rx(3,2) - Tx(2))^2); % example distance
% time3 = distance3/c;

% distance difference
delta_r12 = distance2 - distance1;
delta_r13 = distance3 - distance1;

% calculate delays
delay_r12 = delta_r12 / c;
delay_r13 = delta_r13 / c;

% 
% % distance difference using times
% delta_r12 = c*(time2 - time1);
% delta_r13 = c*(time3 - time1);
% 
% t12 = delta_r12 / c;
% t13 = delta_r13 / c;
% t23 = delta_r23 / c;

% % ------------------------------------------------------------------------
% Cross - correlation
% % ------------------------------------------------------------------------

lag_r12 = delay_r12 * Sample_Rate;
lag_r13 = delay_r13 * Sample_Rate;

lag_r12 = round(lag_r12);
lag_r13 = round(lag_r13);

% Generate signals
% Define parameters
fs = 44100; % Sampling frequency (Hz)
duration = 1; % Duration of the signal in seconds
f_tone = 1000; % Frequency of the tone (Hz)
amplitude = 1; % Amplitude of the tone
noise_level = 0.2; % Level of random noise

t = linspace(0, duration, fs * duration);
tone_signal = amplitude * sin(2 * pi * f_tone * t);
noise = noise_level * randn(1, length(t));
signal_with_noise = tone_signal + noise;
Signal = signal_with_noise;

% Input readings for real scenario
% Rx1 = 
% Rx2 = 
% Rx3 = 

max_lag = 0;

if(lag_r12<0&&lag_r13<0)
    if(abs(lag_r12)>abs(lag_r13))
        max_lag = lag_r12;
        Rx2 = Signal;
        Rx1 = [zeros(1,abs(max_lag)),Rx2];
        Rx3 = [zeros(1,abs(max_lag)-abs(lag_r13)),Rx2];
    elseif(abs(lag_r13)>abs(lag_r12))
        max_lag = lag_r13;
        Rx3 = Signal;
        Rx1 = [zeros(1,abs(max_lag)),Rx3];
        Rx2 = [zeros(1,abs(max_lag)-abs(lag_r12)),Rx3];
    end
elseif (lag_r12<0&&lag_r13>=0)
    Rx2 = Signal;
    Rx1 = [zeros(1,abs(lag_r12)),Rx2];
    Rx3 = [zeros(1,lag_r12),Rx1];
elseif (lag_r12>=0&&lag_r13<0)
    Rx3 = Signal;
    Rx1 = [zeros(1,abs(lag_r13)),Rx3];
    Rx2 = [zeros(1,lag_r12),Rx1];
else
    Rx1 = Signal;
    Rx2 = [zeros(1,lag_r12),Rx1];
    Rx3 = [zeros(1,lag_r13),Rx1];
end


% Perform cross-correlation
[correlation, lag] = xcorr(Rx2, Rx1);
[correlation2, lag2] = xcorr(Rx3, Rx1);

% Find the index of the maximum correlation value
[~, idx] = max(correlation);
[~, idx2] = max(correlation2);

% Calculate the time shift
lag = lag(idx);
lag2 = lag2(idx2);

% calculate delay

delay_r12 = lag/Sample_Rate;
delay_r13 = lag2/Sample_Rate;

% calculate distance
delta_r12 = c * delay_r12;
delta_r13 = c * delay_r13;

% Lag represents the time difference in number of samples

% disp(['Maximum cross-correlation value: ' num2str(max_corr)]);
disp(['Lag between signals: ' num2str(lag) ' Samples']);
disp(['Lag between Rx12: ' num2str(lag_r12) ' Samples']);
% delay = lag/Sample_Rate;
% disp(['Delay between signals: ' num2str(delay) ' seconds']);
% disp(['Maximum cross-correlation value 2: ' num2str(max_corr2)]);
disp(['Lag between signals 2: ' num2str(lag2) ' Samples']);
disp(['Lag between Rx13: ' num2str(lag_r13) ' Samples']);
% delay2 = lag2/Sample_Rate;
% disp(['Delay between signals 2: ' num2str(delay2) ' seconds']);


% ------------------------------------------------------------------------
% Least Squares Method
% Define the objective function

objective = @(xy) [
    sqrt((Rx(2,1) - xy(1))^2 + (Rx(2,2) - xy(2))^2) - sqrt((Rx(1,1) - xy(1))^2 + (Rx(1,2) - xy(2))^2) - delta_r12;
    sqrt((Rx(3,1) - xy(1))^2 + (Rx(3,2) - xy(2))^2) - sqrt((Rx(1,1) - xy(1))^2 + (Rx(1,2) - xy(2))^2) - delta_r13
];
tic;


    % Initial guess for transmitter position
    x0 = [D*rand(), D*rand()]; % Random initial guess within the room dimensions
    
    % Use least squares optimization to minimize the objective function
    % options = optimset('Display','iter');
    transmitter_position = lsqnonlin(objective, x0, [], []);
    calculated(i,:) = transmitter_position;
    x = transmitter_position(1);
    y = transmitter_position(2);
    ls = toc;


% disp('Estimated source position:');
% disp(brute_force);
% disp('Actual source position:');
% disp(Tx);
% disp('Error:');
% disp(abs(sqrt((Tx(1) - brute_force(1))^2 + (Tx(2) - brute_force(2))^2)));
% disp(['Brute Force approach took: ', num2str(bf), ' seconds']);

disp('Estimated source position:');
disp([x y]);
disp('Actual source position:');
disp(Tx);
disp('Error:');
format longg;
disp(abs(sqrt((Tx(1) - x)^2 + (Tx(2) - y)^2)));
disp(['Least Squares approach took: ', num2str(ls), ' seconds']);
end

for i= 1:1:14
pause(0.70);

figure(1); clf; hold on;

p(1) = plot(Rx(1,1), Rx(1,2), 'k.');
p(2) = plot(Rx(2,1), Rx(2,2), 'k.');
p(3) = plot(Rx(3,1), Rx(3,2), 'k.');
p(4) = plot(real(:,1), real(:,2), 'go'); % Actual Tx position

% ------------------------------------------------------------------------
% Plot the results

% Rx positions
% figure(1); clf; hold on;
% zfun = @(x,y) sqrt((Rx(2,1) - x).^2 + (Rx(2,2) - y).^2) - sqrt((Rx(1,1) - x).^2 + (Rx(1,2) - y).^2);
% zfun2 = @(x,y) sqrt((Rx(3,1) - x)^2 + (Rx(3,2) - y)^2) - sqrt((Rx(1,1) - x)^2 + (Rx(1,2) - y)^2);
% zfun3 = @(x,y) sqrt((Rx(3,1) - x)^2 + (Rx(3,2) - y)^2) - sqrt((Rx(2,1) - x)^2 + (Rx(2,2) - y)^2);
% 
% fcontour(zfun, 'LineColor', 'b', 'LevelList', 2, 'LineWidth', 1, 'LineStyle', '--');
% fcontour(zfun2, 'LineColor', 'b', 'LevelList', 4, 'LineWidth', 1, 'LineStyle', '--');
% fcontour(zfun3, 'LineColor', 'b', 'LevelList', -2, 'LineWidth', 1, 'LineStyle', '--');

% 
% p(1) = plot(Rx(1,1), Rx(1,2), 'k.');
% p(2) = plot(Rx(2,1), Rx(2,2), 'k.');
% p(3) = plot(Rx(3,1), Rx(3,2), 'k.');

% Tx position
% p(4) = plot(calculated(:,1), calculated(:,2), 'go'); % Actual Tx position

p(5) = plot(calculated(i,1),calculated(i,2), 'rdiamond'); % Calculated Tx position

p(1).MarkerSize = 20;
p(2).MarkerSize = 20;
p(3).MarkerSize = 20;
p(4).MarkerSize = 6; p(4).LineWidth = 1.5;
p(5).MarkerSize = 6; p(5).LineWidth = 1.5;

xlabel('X-axis (meters)');
ylabel('Y-axis (meters)');
title('Time Difference of Arrival');

% Plot lines for fun

L1 = line([Rx(1,1) calculated(i,1)], [Rx(1,2) calculated(i,2)], 'Color', 'blue', 'LineStyle', '--');
L2 = line([Rx(2,1) calculated(i,1)], [Rx(2,2) calculated(i,2)], 'Color', 'blue', 'LineStyle', '--');
L3 = line([Rx(3,1) calculated(i,1)], [Rx(3,2) calculated(i,2)], 'Color', 'blue', 'LineStyle', '--');

% legend([p(1), p(2), p(3), p(4), p(5)], 'Rx 1', 'Rx 2', 'Rx 3', 'Tx Actual', 'Tx Calculated', 'NumColumns', 1);

hold off;
end
% % Find all values which the difference between distance 1 and distance 2 is
% 


