clear; clc;
% 定义三个方波一个周期
x1 = ones(1,8);                       
x2 = [ones(1,8),  zeros(1,8)];       
x3 = [ones(1,8),  zeros(1,24)];       

N1=8; N2=16; N3=32;

figure;
subplot(3,1,1);
stem(0:63, repmat(x1,1,N1/N1*8));  
title('x1[n], N1=8'); xlabel('n'); grid on;

subplot(3,1,2);
stem(0:63, repmat(x2,1,4));         
title('x2[n], N2=16'); xlabel('n'); grid on;

subplot(3,1,3);
stem(0:63, repmat(x3,1,2));         
title('x3[n], N3=32'); xlabel('n'); grid on;