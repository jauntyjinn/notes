clear; clc; close all;
n = 0:5;
x1 = [1, 0, 0, 0, 0, 0]; 
x2 = [0, 1, 0, 0, 0, 0];  
x3 = x1 + 2 * x2;         

% 1
b = [1, -1, -1]; a = 1;
w1 = filter(b, a, x1);
w2 = filter(b, a, x2);
w3 = filter(b, a, x3);

figure(1);
subplot(2,2,1); stem(n, w1); title('w1[n]');
subplot(2,2,2); stem(n, w2); title('w2[n]');
subplot(2,2,3); stem(n, w3); title('w3[n]');
subplot(2,2,4); stem(n, w1 + 2.*w2); title('w1[n] + 2*w2[n]');

% 2
y1 = cos(x1); y2 = cos(x2); y3 = cos(x3);

figure(2);
subplot(2,2,1); stem(n, y1); title('y1[n]');
subplot(2,2,2); stem(n, y2); title('y2[n]');
subplot(2,2,3); stem(n, y3); title('y3[n]');
subplot(2,2,4); stem(n, y1 + 2.*y2); title('y1[n] + 2*y2[n]');

% 3
z1 = n .* x1;   z2 = n .* x2;   z3 = n .* x3; %点乘

figure(3);
subplot(2,2,1); stem(n, z1); title('z1[n]');
subplot(2,2,2); stem(n, z2); title('z2[n]');
subplot(2,2,3); stem(n, z3); title('z3[n]');
subplot(2,2,4); stem(n, z1 + 2.*z2); title('z1[n] + 2*z2[n]');