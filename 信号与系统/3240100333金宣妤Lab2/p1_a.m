clear; clc;
figure;

b1 = [1 5];
a1 = [1 2 3];
zs1 = roots(b1);
ps1 = roots(a1);

subplot(1, 3, 1); 
plot(real(zs1), imag(zs1), 'o');
hold on
plot(real(ps1), imag(ps1), 'x');
grid on;
title('1(a)(i)');
xlabel('Real'); ylabel('Imaginary');

b2 = [2 5 12];
a2 = [1 2 10];
zs2 = roots(b2);
ps2 = roots(a2);

subplot(1, 3, 2); 
plot(real(zs2), imag(zs2), 'o');
hold on
plot(real(ps2), imag(ps2), 'x');
grid on;
title('1(a)(ii)');
xlabel('Real'); ylabel('Imaginary');

b3 = [2 5 12];
a3 = conv([1 2 10], [1 2]); 
zs3 = roots(b3);
ps3 = roots(a3);

subplot(1, 3, 3); 
plot(real(zs3), imag(zs3), 'o');
hold on
plot(real(ps3), imag(ps3), 'x');
grid on;
title('1(a)(iii)');
xlabel('Real'); ylabel('Imaginary');