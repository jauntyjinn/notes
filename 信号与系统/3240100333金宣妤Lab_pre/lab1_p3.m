clear; clc;
n = 0:31;
x1 = sin(pi*n/4) .* cos(pi*n/4);
x2 = cos(pi*n/4).^2;
x3 = sin(pi*n/4) .* cos(pi*n/8);

figure;
subplot(3, 1, 1); stem(n, x1, 'filled'); title('x_1[n]');
subplot(3, 1, 2); stem(n, x2, 'filled'); title('x_2[n]');
subplot(3, 1, 3); stem(n, x3, 'filled'); title('x_3[n]');