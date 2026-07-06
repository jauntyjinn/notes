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

u = [1, zeros(1,20)];  
u_step = ones(1,21);
n_step = 0:20;

figure;
s1 = filter(b1,a1,u_step);
s2 = filter(b2,a2,u_step);
s3 = filter(b3,a3,u_step);

subplot(3,1,1); stem(n_step,s1); title('Filter1 阶跃响应'); grid on;
subplot(3,1,2); stem(n_step,s2); title('Filter2 阶跃响应'); grid on;
subplot(3,1,3); stem(n_step,s3); title('Filter3 阶跃响应'); grid on;