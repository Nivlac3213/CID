% Cleanup
% clear all;
close all;
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
% 
% Rx receiver positions
Rx = [0,0;D,0;.25*D,D];
real = Tx;
calculated = zeros(1,2);



for i=1:1:14

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
delta_r23 = distance3 - distance2;


% 
% % distance difference using times
% delta_r12 = c*(time2 - time1);
% delta_r13 = c*(time3 - time1);
% 
% t12 = delta_r12 / c;
% t13 = delta_r13 / c;
% t23 = delta_r23 / c;

% % ------------------------------------------------------------------------
% 
% % Math
% 
% % Brute force approach
% tol = 0.003; % Tolerance
% tic;
% 
% x = D*abs(rand());
% y = D*abs(rand());
% 
% % Difference in distances to Tx of Rx 1 & 2
% eq12 = sqrt((Rx(2,1)-x)^2 + (Rx(2,2)-y)^2) - sqrt((Rx(1,1)-x)^2 + (Rx(1,2)-y)^2) - delta_r12; 
% % Difference in distances to Tx of Rx 1 & 3
% eq13 = sqrt((Rx(3,1)-x)^2 + (Rx(3,2)-y)^2) - sqrt((Rx(1,1)-x)^2 + (Rx(1,2)-y)^2) - delta_r13;
% iter = 0; % iterations
% while abs(eq12) > tol || abs(eq13) > tol
%     iter = iter + 1;
%     x = D*abs(rand());
%     y = D*abs(rand());
% 
%     % Difference in distances to Tx of Rx 1 & 2
%     eq12 = sqrt((Rx(2,1)-x)^2 + (Rx(2,2)-y)^2) - sqrt((Rx(1,1)-x)^2 + (Rx(1,2)-y)^2) - delta_r12; 
%     % Difference in distances to Tx of Rx 1 & 3
%     eq13 = sqrt((Rx(3,1)-x)^2 + (Rx(3,2)-y)^2) - sqrt((Rx(1,1)-x)^2 + (Rx(1,2)-y)^2) - delta_r13;
% 
% end
% bf = toc;
% brute_force = [x y];


% Bisection method

% % Define the equations
% f = @(x, y) sqrt((Rx(2,1)-x)^2 + (Rx(2,2)-y)^2) - sqrt((Rx(1,1)-x)^2 + (Rx(1,2)-y)^2) - delta_r12;  % Example equation 1
% g = @(x, y) sqrt((Rx(3,1)-x)^2 + (Rx(3,2)-y)^2) - sqrt((Rx(1,1)-x)^2 + (Rx(1,2)-y)^2) - delta_r13;       % Example equation 2
% 
% % Define the bisection search parameters
% tol = 1; % Tolerance for convergence
% max_iter = 10000; % Maximum number of iterations
% 
% % Initial guess for the interval
% x_lower = 0;
% x_upper = 10;
% y_lower = 0;
% y_upper = 10;
% 
% % Bisection search loop
% for iter = 1:max_iter
% 
%     % Bisection step
%     x_mid = (x_lower + x_upper) / 2;
%     y_mid = (y_lower + y_upper) / 2;
% 
%     % Evaluate the equations at the midpoints
%     f_mid = f(x_mid, y_mid);
%     g_mid = g(x_mid, y_mid);
% 
%     % Check for convergence
%     if abs(f_mid) < tol && abs(g_mid) < tol
%         x_solution = x_mid;
%         y_solution = y_mid;
%         disp(['Converged in ', num2str(iter), ' iterations']);
%         return;
%     end
% 
%     % Update the interval based on the signs of f_mid and g_mid
%     if f_mid * f(x_lower, y_lower) < 0
%         x_upper = x_mid;
%         y_upper = y_mid;
%     else
%         x_lower = x_mid;
%         y_lower = y_mid;
%     end
% 
% end
% 
% % If max_iter is reached, display a warning
% warning('Maximum number of iterations reached without convergence');
% 
% disp(x_solution);
% disp(y_solution);

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

legend([p(1), p(2), p(3), p(4), p(5)], 'Rx 1', 'Rx 2', 'Rx 3', 'Tx Actual', 'Tx Calculated', 'NumColumns', 1);



hold off;
end
% % Find all values which the difference between distance 1 and distance 2 is
% 
