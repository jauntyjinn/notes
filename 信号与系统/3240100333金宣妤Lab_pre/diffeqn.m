% diffeqn.m
function y = diffeqn(a, x, yn1)
    N = length(x);
    y = zeros(1, N);
    y(1) = a * yn1 + x(1); 
   
    for n = 2:N
        y(n) = a * y(n-1) + x(n);
    end
end