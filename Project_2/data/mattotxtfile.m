clear
close all


filename = 'mnist_train';
varX     = 'train_X';
varL     = 'train_labels';

%% READ DATA

fprintf('...reading data...\n');
fprintf('   - using file %s\n', filename)

% IO data file
ioData = matfile( [filename '.mat'] );

% read variables
X = ioData.(varX);
L = ioData.(varL);

% number of points
nPoint = size( X, 1 );

fout = 'mnist_train.txt';
dlmwrite(fout,X,'delimiter','\t','precision','%.6f') 