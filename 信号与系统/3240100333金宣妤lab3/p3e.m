clear; clc;
x1 = ones(1,8);                       
x2 = [ones(1,8),  zeros(1,8)];       
x3 = [ones(1,8),  zeros(1,24)];       
N1=8; N2=16; N3=32;

a1 = dtfs(x1, 0);
a2 = dtfs(x2, 0);
a3 = dtfs(x3, 0);

figure;
subplot(3,1,1);
stem(0:N1-1, abs(a1));
title('|a_k| of x1'); xlabel('k'); grid on;

subplot(3,1,2);
stem(0:N2-1, abs(a2));
title('|a_k| of x2'); xlabel('k'); grid on;

subplot(3,1,3);
stem(0:N3-1, abs(a3));
title('|a_k| of x3'); xlabel('k'); grid on;

% DC分量验证
fprintf('a1(k=0) = %.4f, 预测值 = %.4f\n', a1(1), 8/N1);
fprintf('a2(k=0) = %.4f, 预测值 = %.4f\n', a2(1), 8/N2);
fprintf('a3(k=0) = %.4f, 预测值 = %.4f\n', a3(1), 8/N3);