clear; clc;

b_3rd = [1 7 21];
a_3rd = [1 1 24 -26];

t = 0:0.01:4; 

h_ana = exp(t) + exp(-t) .* sin(5*t);
h_sim = impulse(b_3rd, a_3rd, t);

figure;
plot(t, h_ana, 'o');
hold on;
plot(t, h_sim, 'x');
grid on;
