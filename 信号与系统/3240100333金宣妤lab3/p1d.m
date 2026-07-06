clear;
clc;
n=0:99;
x=cos(n.^2).*sin((2/5)*pi.*n);
h=(0.9).^n.*[ones(1,10) zeros(1,90)];

y1=conv(h,x);
y = y1(1:100);  ny = 0:99;

stem(ny,y);
xlabel('n'); ylabel('y[n]');

