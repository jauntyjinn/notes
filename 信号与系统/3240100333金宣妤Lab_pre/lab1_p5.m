clear; clc;
% 非线性证明
n = -5:5;
delta = (n == 0);          % x1[n] = delta[n]
x2 = 2 * delta;            

y1 = sin((pi/2) * delta);
y2 = sin((pi/2) * x2);

figure;
subplot(3, 1, 1); stem(n, 2*y1, 'filled'); title('2 * T\{x_1[n]\}');
subplot(3, 1, 2); stem(n, y2, 'filled'); title('T\{2 * x_1[n]\}');

% 非因果证明
n_in = -5:9;
x = (n_in >= 0);           %  x[n] = u[n]
n_out = -6:9;
y = zeros(size(n_out));

for idx = 1:length(n_out)
    nn = n_out(idx);
    xn = (nn >= 0); 
    xn_plus_1 = (nn+1 >= 0);
    y(idx) = xn + xn_plus_1;
end

subplot(3, 1, 3); 
stem(n_out, y, 'filled'); 
title('Output y[n] = x[n] + x[n+1]'); xlabel('n');