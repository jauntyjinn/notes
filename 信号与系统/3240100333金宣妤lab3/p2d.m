clear; clc;
load plus.mat

% 原始图像
% figure;
% colormap(gray); image(64*x);
% title('原始图像');

wc = 0.4;
n1 = 10; n2 = 4; n3 = 12;
[b1,a1] = butter(n1, wc);
a2 = 1; b2 = remez(n2, [0 wc-0.04 wc+0.04 1], [1 1 0 0]);
a3 = 1; b3 = remez(n3, [0 wc-0.04 wc+0.04 1], [1 1 0 0]);

x16 = x(:,16);

y16_f1 = filter(b1,a1,[x16; zeros(n1/2,1)]);
y16_f1 = y16_f1(n1/2+1:end);   

figure;
plot(x16,'b'); hold on; plot(y16_f1,'r');
legend('原始x16','滤波后y16');
title('Filter1对第16列的响应'); grid on;