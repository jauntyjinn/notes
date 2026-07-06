clear; clc;
N = 6;

% x1[n]: 周期信号，周期为12。绘制两个周期 (0~23)
n1 = 0:23;
x1 = cos(2*pi*n1/N) + 2*cos(3*pi*n1/N);
figure;
subplot(3, 1, 1); stem(n1, x1, 'filled'); 
title('x_1[n]'); xlabel('n');

% x2[n]: 非周期信号。绘制 0~4N (0~24)
n2 = 0:24;
x2 = 2*cos(2*n2/N) + cos(3*n2/N);
subplot(3, 1, 2); stem(n2, x2, 'filled'); 
title('x_2[n]'); xlabel('n');

% x3[n]: 周期信号，周期为24。绘制两个周期 (0~47)
n3 = 0:47;
x3 = cos(2*pi*n3/N) + 3*sin(5*pi*n3/(2*N));
subplot(3, 1, 3); stem(n3, x3, 'filled'); 
title('x_3[n]'); xlabel('n');