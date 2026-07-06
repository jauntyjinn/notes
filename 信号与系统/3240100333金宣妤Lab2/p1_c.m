clear; clc;

b = [1 2 5];
a = [1 -3];
[ps, zs] = pzplot(b,a,4);
% --- 下面是手动修补阴影的代码 ---
hold on;
yl = ylim;  xl = xlim; 

fill([3, xl(2), xl(2), 3], [yl(1), yl(1), yl(2), yl(2)], [0.9 0.9 0.9], ...
    'FaceAlpha', 0.5, 'EdgeColor', 'none');

uistack(findobj(gca, 'Type', 'Patch'), 'bottom');