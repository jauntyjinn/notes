clear;clc; 

a = [1, 2];  
b = 1;       

t = 0:0.5:10; 
x = (t >= 2) * 1; 
y_sim = lsim(b, a, x, t);

plot(t, y_sim); 

