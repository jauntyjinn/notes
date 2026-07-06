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

figure;
[H1,w1] = freqz(b1,a1,512);
[H2,w2] = freqz(b2,a2,512);
[H3,w3] = freqz(b3,a3,512);

subplot(3,2,1); plot(w1/pi, abs(H1));
xlabel('\omega/\pi'); ylabel('|H|'); title('Filter1(Butter) 幅频'); grid on;
subplot(3,2,2); plot(w1/pi, angle(H1));
xlabel('\omega/\pi'); ylabel('Phase'); title('Filter1(Butter) 相频'); grid on;

subplot(3,2,3); plot(w2/pi, abs(H2));
xlabel('\omega/\pi'); ylabel('|H|'); title('Filter2(Remez n=4) 幅频'); grid on;
subplot(3,2,4); plot(w2/pi, angle(H2));
xlabel('\omega/\pi'); ylabel('Phase'); title('Filter2(Remez n=4) 相频'); grid on;

subplot(3,2,5); plot(w3/pi, abs(H3));
xlabel('\omega/\pi'); ylabel('|H|'); title('Filter3(Remez n=12) 幅频'); grid on;
subplot(3,2,6); plot(w3/pi, angle(H3));
xlabel('\omega/\pi'); ylabel('Phase'); title('Filter3(Remez n=12) 相频'); grid on;