clear; clc;

t = -10:0.01:10;
x = double((t >= -3) & (t <= 2)); 

b1 = 5;
a1 = [1 2 26];
y1 = lsim(b1, a1, x, t);

b2_rev = 1;
a2_rev = [-1 -1];
r = flip(x); 
w = lsim(b2_rev, a2_rev, r, t);
y2 = flip(w);

y = y1 + y2;

figure;
subplot(3, 1, 1);
plot(t, y1, 'b');
grid on;
title('y_1(t)');

subplot(3, 1, 2);
plot(t, y2, 'r');
grid on;
title('y_2(t)');

subplot(3, 1, 3);
plot(t, y, 'k'); 
grid on;
title('y(t) = y_1(t) + y_2(t)');
