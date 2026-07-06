clear; clc; close all;

delta = 0.05;
t = -1:0.05:4;
a = [1 3];
b = 1;

dt1 = 0.1;
dt2 = 0.2;
dt3 = 0.4;

d1 = zeros(1, length(t));
d1(1, 1/delta+1 : 1/delta+dt1/delta) = 1/dt1;

d2 = zeros(1, length(t));
d2(1, 1/delta+1 : 1/delta+dt2/delta) = 1/dt2;

d3 = zeros(1, length(t));
d3(1, 1/delta+1 : 1/delta+dt3/delta) = 1/dt3;

u = (t >= 0); 
h = exp(-3.*t) .* u; 

h1 = lsim(b, a, d1, t);
h2 = lsim(b, a, d2, t);
h3 = lsim(b, a, d3, t);

plot(t, h1); hold on;
plot(t, h2); hold on;
plot(t, h3); hold on;
plot(t, h); 
