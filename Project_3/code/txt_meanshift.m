%
% SCRIPT: DEMO_MEANSHIFT
%
%   Sample script on usage of mean-shift function.
%
% DEPENDENCIES
%
%   meanshift
%
%


%% CLEAN-UP

clear;
close all;


%% PARAMETERS

% dataset options
basepath = '.';
filename = 'Dim_256x64';


% mean shift options
h = 10;
optMeanShift.epsilon = 1e-4*h;
optMeanShift.verbose = true;
optMeanShift.display = false;


%% (BEGIN)

fprintf('\n *** begin %s ***\n\n',mfilename);


%% READ DATA

fprintf('...reading data...\n')

txtFile = [basepath filesep filename '.txt'];

fprintf('   - file: %s...\n', txtFile)

fileID = fopen(txtFile,'r');
x = fscanf(fileID,'%f',[64 256]);
x = x';

labels = fopen('DIM_labels_256.txt','r');
l = fscanf(labels,'%d');

figure('name', 'original_data')
scatter(x(:,1),x(:,2),8,l);


%% PERFORM MEAN SHIFT


fprintf('...computing mean shift...')

tic;
y = meanshift( x, h, optMeanShift );
tElapsed = toc;

fprintf('DONE in %.2f sec\n', tElapsed);


%% SHOW FINAL POSITIONS

figure('name', 'final_local_maxima_points')
scatter(y(:,1),y(:,2),8,l);

fout = 'validation_DIM_256x64.txt';
dlmwrite(fout,y,'delimiter','\t','precision','%f') 
%% (END)

fprintf('\n *** end %s ***\n\n',mfilename);


%%------------------------------------------------------------
%
% AUTHORS
%
%   Dimitris Floros                         fcdimitr@auth.gr
%
% VERSION
%
%   0.1 - December 29, 2017
%
% CHANGELOG
%
%   0.1 (Dec 29, 2017) - Dimitris
%       * initial implementation
%
% ------------------------------------------------------------
