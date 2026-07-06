clear; clc;

a = [-1 2]; 
b = 1;    

t = -5:0.01:5;

h_ana = -exp(-2*t) .* (t <= 0); 

t1 = 0:0.01:5; 
h = impulse(b, a, t1);

hc = [zeros(length(t)-length(h), 1); h];
hc = flip(hc);

plot(t, h_ana, 'o'); 
hold on;
plot(t, hc, 'x'); 
