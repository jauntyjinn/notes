clear;
clc;

nh=-1:1;h=[2 0 -2];
nx=0:2;x=[1 0 1];
ny=-1:3;y=conv(h,x);
stem(ny,y);
xlabel('n'); ylabel('y[n]');