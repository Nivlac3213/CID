% Cleanup
clear all;
close all;
% ------------------------------------------------------------------------
% Initialization
D = 20; % dimensions of a room
nRx = 3; % Number of receivers
nTx = 1; % Number of emitters
dim = 2; % Number of dimensions for the problem
Rx = D*rand(nRx, dim); % Rx locations x, y in meters
Rx = abs(Rx); % only pos coordinates
Tx = D*rand(nTx, dim); % Tx location x, y in meters
Tx = abs(Tx); % only pos coordinates
c = 299792458; % speed of light in m/s

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

% These two lines break everything
% delta_r12 = abs(delta_r12); %only positive difference 
% delta_r13 = abs(delta_r13); %only positive difference

% ------------------------------------------------------------------------

% Math

% Brute force approach
tol = 0.009; % Tolerance

x = D*abs(rand());
y = D*abs(rand());

% Difference in distances to Tx of Rx 1 & 2
eq12 = sqrt((Rx(2,1)-x)^2 + (Rx(2,2)-y)^2) - sqrt((Rx(1,1)-x)^2 + (Rx(1,2)-y)^2) - delta_r12; 
% Difference in distances to Tx of Rx 1 & 3
eq13 = sqrt((Rx(3,1)-x)^2 + (Rx(3,2)-y)^2) - sqrt((Rx(1,1)-x)^2 + (Rx(1,2)-y)^2) - delta_r13;

while abs(eq12) > tol || abs(eq13) > tol
    x = D*abs(rand());
    y = D*abs(rand());

    % Difference in distances to Tx of Rx 1 & 2
    eq12 = sqrt((Rx(2,1)-x)^2 + (Rx(2,2)-y)^2) - sqrt((Rx(1,1)-x)^2 + (Rx(1,2)-y)^2) - delta_r12; 
    % Difference in distances to Tx of Rx 1 & 3
    eq13 = sqrt((Rx(3,1)-x)^2 + (Rx(3,2)-y)^2) - sqrt((Rx(1,1)-x)^2 + (Rx(1,2)-y)^2) - delta_r13;

end

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
% Plot the results

% Rx positions
figure(1); clf; hold on;
p(1) = plot(Rx(1,1), Rx(1,2), 'k.');
p(2) = plot(Rx(2,1), Rx(2,2), 'k.');
p(3) = plot(Rx(3,1), Rx(3,2), 'k.');

% Tx position
p(4) = plot(Tx(1,1), Tx(1,2), 'bdiamond'); % Actual Tx position
p(5) = plot(x,y, 'rdiamond'); % Calculated Tx position

p(1).MarkerSize = 20;
p(2).MarkerSize = 20;
p(3).MarkerSize = 20;
p(4).MarkerSize = 6; p(4).LineWidth = 1.5;
p(5).MarkerSize = 6; p(5).LineWidth = 1.5;

xlabel('X-axis (meters)');
ylabel('Y-axis (meters)');
title('Time Distance of Arrival');

% legend([p(1), p(2), p(3), p(4), p(5)], 'Rx1', 'Rx2', 'Rx3', 'Tx Actual', 'Tx Calculated', 'NumColumns', 2);

% Plot lines for fun

line([Rx(1,1) x], [Rx(1,2) y], 'Color', 'blue', 'LineStyle', '--');
line([Rx(2,1) x], [Rx(2,2) y], 'Color', 'blue', 'LineStyle', '--');
line([Rx(3,1) x], [Rx(3,2) y], 'Color', 'blue', 'LineStyle', '--');








