clear; clc;

b_3rd = [1 7 21];
a_3rd = [1 1 24 -26];

[r, p, k] = residue(b_3rd, a_3rd);

disp('Residues (r):'); 
disp(r);
disp('Poles (p):'); 
disp(p);
disp('Direct Term (k):'); 
disp(k);