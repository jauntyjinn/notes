clear; clc; close all;

a5 = [1, 0.5];
b5 = 1;
t5 = 0:0.1:10;

s_exact = 2 * (1 - exp(-t5/2));             
y_lsim = lsim(b5, a5, ones(size(t5)), t5);  
y_step = step(b5, a5, t5); 

figure(1);
plot(t5, s_exact, 'r-', t5, y_lsim, 'b--', t5, y_step, 'k:'); 
title('Step Response');
legend('Exact s(t)', 'lsim', 'step()', 'Location', 'Southeast');
xlabel('t'); ylabel('s(t)'); grid on;

h_exact = exp(-t5/2);                       
y_impulse = impulse(b5, a5, t5);
figure(2);
plot(t5, h_exact, 'r-', t5, y_impulse, 'b--'); 
title('Impulse Response');
legend('Exact h(t)', 'impulse()');
xlabel('t'); ylabel('h(t)'); grid on;