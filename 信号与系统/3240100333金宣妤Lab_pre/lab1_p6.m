clear; clc;
% Problem 6 (b)
n = 0:30;

x1 = (n == 0);  % delta[n]
x2 = (n >= 0);  % u[n]

y1 = diffeqn(1, x1, 0);
y2 = diffeqn(1, x2, 0);

figure;
subplot(2, 1, 1); stem(n, y1, 'filled'); title('Response to \delta[n]');
subplot(2, 1, 2); stem(n, y2, 'filled'); title('Response to u[n]');