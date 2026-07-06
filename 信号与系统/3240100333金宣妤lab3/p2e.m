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

y16_f2 = filter(b2,a2,[x16; zeros(n2/2,1)]);
y16_f2 = y16_f2(n2/2+1:end);

y16_f3 = filter(b3,a3,[x16; zeros(n3/2,1)]);
y16_f3 = y16_f3(n3/2+1:end);

figure;
subplot(3,1,1); plot(x16,'b');    title('原始x16'); grid on;
subplot(3,1,2); plot(y16_f2,'r'); title('Filter2输出'); grid on;
subplot(3,1,3); plot(y16_f3,'r'); title('Filter3输出'); grid on;