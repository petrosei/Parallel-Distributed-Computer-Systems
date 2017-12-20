clear
close all

K = 30;
N = 10000;
D = 784;

x = 1:N;

fin = 'mnist_train.txt';
delimiterIn = '\t';
A = importdata(fin,delimiterIn);

tic;
[TDX,D] = knnsearch(A,A,'K',K+1,'NSMethod','euclidean');
fprintf('DONE in %.2f sec\n', toc);
fout = 'validation_mnist_train.txt';
D = D.^2;
dlmwrite(fout,D(:,2:K+1),'delimiter','\t','precision','%.6f') 