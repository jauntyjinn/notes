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

y1 = filt2d(b1, a1, n1/2, x);
y2 = filt2d(b2, a2, n2/2, x);
y3 = filt2d(b3, a3, n3/2, x);

figure;
colormap(gray);
subplot(1,4,1); image(64*x);  title('原始图像');
subplot(1,4,2); image(64*y1); title('Filter1(Butter)');
subplot(1,4,3); image(64*y2); title('Filter2(Remez 4)');
subplot(1,4,4); image(64*y3); title('Filter3(Remez 12)');