clear; clc;
N = 5;
a = [1, 2*exp(-1j*pi/3), exp(1j*pi/4), exp(-1j*pi/4), 2*exp(1j*pi/3)];

n = 0:N-1;
x = zeros(1,N);
for k = 0:N-1
    x = x + a(k+1)*exp(1j*k*(2*pi/N)*n);
end
x = real(x);  

figure;
stem(n, x);
xlabel('n'); ylabel('x[n]');
title('Problem3 Basic: 合成信号 x[n], N=5');

fprintf('x = %.4f  %.4f  %.4f  %.4f  %.4f\n', x(1),x(2),x(3),x(4),x(5));