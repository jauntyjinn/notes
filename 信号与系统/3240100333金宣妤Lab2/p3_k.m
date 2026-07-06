clear; clc;

t = -4:0.01:4;
t_pos = 0:0.01:4; 

h_ana = ( -exp(t) - exp(-t).*sin(5*t) ) .* (t <= 0);

a_rev = [-1  1 -24 -26]; 
b_rev = [ 1 -7  21];     

h_rev_pos = impulse(b_rev, a_rev, t_pos);

lh = length(h_rev_pos);
h_sim = [zeros(length(t)-lh, 1); h_rev_pos]; 
h_sim = flip(h_sim);                        

figure;
plot(t, h_ana, 'o', 'LineWidth', 1.5);
hold on;
plot(t, h_sim, 'x', 'LineWidth', 1.5);
grid on;
