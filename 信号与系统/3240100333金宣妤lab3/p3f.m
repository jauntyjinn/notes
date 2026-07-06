clear; clc;
x1 = ones(1,8);                       
x2 = [ones(1,8),  zeros(1,8)];       
x3 = [ones(1,8),  zeros(1,24)];       
N1=8; N2=16; N3=32;
n_plot = 0:31;
a1 = dtfs(x1, 0);
a2 = dtfs(x2, 0);
a3 = dtfs(x3, 0);

x3_2 = zeros(1,N3);
for k=-2:2
    if k>=0, idx=k+1;
    else,    idx=N3+k+1; end
    x3_2 = x3_2 + a3(idx)*exp(1j*k*(2*pi/N3)*n_plot);
end
x3_2 = real(x3_2);

x3_8 = zeros(1,N3);
for k=-8:8
    if k>=0, idx=k+1;
    else,    idx=N3+k+1; end
    x3_8 = x3_8 + a3(idx)*exp(1j*k*(2*pi/N3)*n_plot);
end
x3_8 = real(x3_8);

x3_12 = zeros(1,N3);
for k=-12:12
    if k>=0, idx=k+1;
    else,    idx=N3+k+1; end
    x3_12 = x3_12 + a3(idx)*exp(1j*k*(2*pi/N3)*n_plot);
end
x3_12 = real(x3_12);

x3_all = zeros(1,N3);
for k=-15:16
    if k>=0, idx=k+1;
    else,    idx=N3+k+1; end
    x3_all = x3_all + a3(idx)*exp(1j*k*(2*pi/N3)*n_plot);
end
x3_all = real(x3_all);

% 绘图
figure;
subplot(4,1,1); stem(n_plot, x3_2);   title('x3\_2[n]:  k=-2到2');   xlabel('n'); grid on;
subplot(4,1,2); stem(n_plot, x3_8);   title('x3\_8[n]:  k=-8到8');   xlabel('n'); grid on;
subplot(4,1,3); stem(n_plot, x3_12);  title('x3\_12[n]: k=-12到12'); xlabel('n'); grid on;
subplot(4,1,4); stem(n_plot, x3_all); title('x3\_all[n]: 全部系数'); xlabel('n'); grid on;