clear; clc;
syms t T

x = sin(2*pi*t/T);
x5 = subs(x, T, 5); 

figure; 
ezplot(x5, [0, 10]); 
title('Problem 7: x(t) = sin(2\pi t / 5)');