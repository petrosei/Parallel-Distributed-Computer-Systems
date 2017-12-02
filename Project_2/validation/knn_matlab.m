clear
close all

K = 5;
N = 10000;
D = 100;

x = 1:N;

fin = '../serial/corpus.txt';
delimiterIn = '\t';
A = importdata(fin,delimiterIn);


[TDX,D] = knnsearch(A,A,'K',K+1,'NSMethod','euclidean');
fout = 'validate.txt';
D = D.^2;
dlmwrite(fout,D(:,2:K+1),'delimiter','\t','precision','%.6f') 