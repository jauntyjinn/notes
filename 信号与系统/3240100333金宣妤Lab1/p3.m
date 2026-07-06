clear; clc; close all;

alpha = 0.5;
N = 1000;

he = [1, zeros(1, N-1), alpha];

figure(1);
stem(0:N, he);

a = [1, zeros(1, N-1), alpha];
b = 1;

d = [1, zeros(1, 4000)];
n = 0:4000;
her = filter(b, a, d); 

figure(2);
plot(n, her); 

load lineup.mat  

z = filter(b, a, y);

figure(3);
subplot(2,1,1); plot(y); title('Original');
subplot(2,1,2); plot(z); title('Processed');

sound(z, 8192);