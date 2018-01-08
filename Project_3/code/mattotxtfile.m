clear
close all


% dataset options
basepath = '.';
filename = 'r15';
varX     = 'X';
varL     = 'L';
%% READ DATA

fprintf('...reading data...\n');
fprintf('   - using file %s\n', filename)

matFile = [basepath filesep filename '.mat'];

fprintf('   - file: %s...\n', matFile)

ioData = matfile( matFile );

X = ioData.(varX);
L = ioData.(varL);


fout = 'r15.txt';
dlmwrite(fout,X,'delimiter','\t','precision','%.4f') 