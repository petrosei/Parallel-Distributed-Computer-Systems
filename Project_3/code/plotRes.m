clear;
close all;


%origdata = '../data/r15/r15.txt';
%origdata = '../data/S_set/S_set_5000x2.txt';
origdata = '../data/Dim/Dim_256x64.txt';

fileID1 = fopen(origdata,'r');

%valdata = '../data/r15/validation_r15.txt';
%valdata = '../data/S_set/validation_S_set_5000x2.txt';
valdata = '../data/Dim/validation_Dim_256x64.txt';

fileID2 = fopen(valdata,'r');

%seryoutdata = '../serial/serial_r15/yout.txt';
%seryoutdata = '../serial/serial_S_set/yout.txt';
seryoutdata = '../serial/serial_Dim_256/yout.txt';

fileID3 = fopen(seryoutdata,'r');


%cudayoutdata = '../cuda/cuda_r15/yout.txt';
%cudayoutdata = '../cuda/cuda_S_set/yout.txt';
cudayoutdata = '../cuda/cuda_Dim_256/yout.txt';

fileID4 = fopen(cudayoutdata,'r');



%cuda_sharedyoutdata = '../cuda_shared/cuda_shared_r15/yout.txt';
%cuda_sharedyoutdata = '../cuda_shared/cuda_shared_S_set/yout.txt';
cuda_sharedyoutdata = '../cuda_shared/cuda_shared_Dim_256/yout.txt';

fileID5 = fopen(cuda_sharedyoutdata,'r');



%a = fscanf(fileID1,'%f',[2 600]);
%a = fscanf(fileID1,'%f',[2 50000]);
a = fscanf(fileID1,'%f',[64 256]);
a = a';

%b = fscanf(fileID2,'%f',[2 600]);
%b = fscanf(fileID2,'%f',[2 50000]);
b = fscanf(fileID2,'%f',[64 256]);
b = b';

%c = fscanf(fileID3,'%f',[2 600]);
%c = fscanf(fileID3,'%f',[2 50000]);
c = fscanf(fileID3,'%f',[64 256]);
c = c';

%d = fscanf(fileID4,'%f',[2 600]);
%d = fscanf(fileID4,'%f',[2 50000]);
d = fscanf(fileID4,'%f',[64 256]);
d = d';

%e = fscanf(fileID5,'%f',[2 600]);
%e = fscanf(fileID5,'%f',[2 50000]);
e = fscanf(fileID5,'%f',[64 256]);
e = e';

figure('name', 'original data')
scatter(a(:,1),a(:,2));


figure('name', 'validation data')
scatter(b(:,1),b(:,2));


figure('name', 'serial final data')
scatter(c(:,1),c(:,2));


figure('name', 'cuda final data')
scatter(d(:,1),d(:,2));


figure('name', 'cuda_shared final data')
scatter(e(:,1),e(:,2));