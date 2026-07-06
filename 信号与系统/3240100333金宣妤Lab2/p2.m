clear; clc;

b2a = [1 -1]; 
a2a = [1 3 2];
subplot(1, 3, 1);
dpzplot(b2a, a2a); 
title('2(a)');

b2b = 1;         
a2b = [1 1 0.5]; 
subplot(1, 3, 2);
dpzplot(b2b, a2b);
title('2(b)');

b2c = [1 0.5];
a2c = [1 -1.25 0.75 -0.125];
subplot(1, 3, 3);
dpzplot(b2c, a2c);
title('2(c)');