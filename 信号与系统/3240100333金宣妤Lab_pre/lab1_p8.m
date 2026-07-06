clear; clc;
syms t

x_complex = exp(1i*2*pi*t/16) + exp(1i*2*pi*t/8);
xr = sreal(x_complex);
xi = simag(x_complex);

figure;
subplot(2, 1, 1); 
ezplot(xr, [0, 32]); 
title('Real Component of x(t)');

subplot(2, 1, 2); 
ezplot(xi, [0, 32]); 
title('Imaginary Component of x(t)');