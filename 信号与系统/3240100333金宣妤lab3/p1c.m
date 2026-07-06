clear;
clc;

nh=0:14; h=ones(1,15);

nx=0:24;  x=(nx>=2).*(0.5.^(nx-2));

ny=0:38;  y=conv(h,x);

stem(ny,y);
xlabel('n'); ylabel('y[n]');