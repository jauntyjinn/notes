clear;
clc;
n=0:99;
x=cos(n.^2).*sin((2/5)*pi.*n);
h=(0.9).^n.*[ones(1,10) zeros(1,90)];

ny=0:198;  y=conv(h,x);

nx0=0:49;   x0=cos(nx0.^2).*sin((2/5)*pi.*nx0);
nx1=nx0+50;   x1=cos(nx1.^2).*sin((2/5)*pi.*nx1);
y0=conv(h,x0);
y1=conv(h,x1);
y_final=[y0 zeros(1,50)]+[zeros(1,50) y1];

stem(n,y(1:100),'r');
hold on
stem(n,y_final(1:100),'b');