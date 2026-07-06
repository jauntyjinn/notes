clear; clc; close all;

t = -1:0.05:4;
u = (t >= 0);

s = (1/3) * (1 - exp(-3.*t)) .* u; 
h = exp(-3.*t) .* u;               

figure(1);
subplot(2,1,1); plot(t, s); title('s(t)'); 
subplot(2,1,2); plot(t, h); title('h(t)');

a = [1, 3]; 
b = 1;
t2 = 0:0.05:4;

figure(2);
subplot(2,1,1); step(b, a, t2); title('step()');
subplot(2,1,2); impulse(b, a, t2); title('impulse()');