clear; clc;

t = -5:0.01:5;
t1 = 0:0.01:5; 

y_ana = (2/9) * (exp(2.5*t) - exp(-2*t)) .* (t <= 0);

a_new = [-1 2];
b_new = 1;

r = exp(-2.5*t1); 
w = lsim(b_new, a_new, r, t1);
lw = length(w);

y_sim = [zeros(length(t)-lw, 1); w]; 
y_sim = flip(y_sim);                 

figure;
plot(t, y_ana, 'o');
hold on;
plot(t, y_sim, 'x');
grid on;
