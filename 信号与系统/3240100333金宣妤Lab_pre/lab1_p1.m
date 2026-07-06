clear; clc;
n = 0:9;
k_vals = [1, 2, 4, 6];

figure;
for i = 1:4
    k = k_vals(i);
    wk = 2*pi*k/5;
    xk = sin(wk * n);
    
    subplot(4, 1, i);
    stem(n, xk, 'filled');
    title(['k = ', num2str(k), ', \omega_k = ', num2str(2*k), '\pi/5']);
    xlabel('n'); ylabel('x_k[n]');
end