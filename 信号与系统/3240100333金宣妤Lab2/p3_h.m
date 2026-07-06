clear; clc;

b_3rd = [1 7 21];
a_3rd = [1 1 24 -26];

zs_3rd = roots(b_3rd);
ps_3rd = roots(a_3rd);

disp('Zeros of H(s):'); disp(zs_3rd);
disp('Poles of H(s):'); disp(ps_3rd);

figure;
plot(real(zs_3rd), imag(zs_3rd), 'o', 'LineWidth', 1.5); 
hold on;
plot(real(ps_3rd), imag(ps_3rd), 'x', 'MarkerSize', 10, 'LineWidth', 1.5);

