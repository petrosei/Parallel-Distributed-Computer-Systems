function create_digram(parallel_data,serial_data,name)


%Create Parallel Data
load(parallel_data);
Mpi_Processes =  Data(:,2).*Data(:,3);
Q = Data(:,1);
Q_Vector = [16 17 18 19 20];
Index = Q-15;
P_Time = Data(:,4);

Time_MPI = ones(length(P_Time)/length(Q_Vector),length(Q_Vector));
Legend_vector = {};
for i=1:size(P_Time)
  Time_MPI(log2(Mpi_Processes(i)),Index(i)) = P_Time(i);
end
Time_MPI


%Create Serial Data
load(serial_data);
Mpi_Processes =  Data(:,2).*Data(:,3);
Q = Data(:,1);
Q_Vector = [16 17 18 19 20];
Index = Q-15;
S_Time = Data(:,4);

Time_Serial = ones(length(S_Time)/length(Q_Vector),length(Q_Vector));
for i=1:size(S_Time)
  Time_Serial(log2(Mpi_Processes(i)),Index(i)) = S_Time(i);
end
Time_Serial

for i=1:size(Time_MPI,1)
  Time_MPI(i,:) = Time_Serial(i,:)./Time_MPI(i,:);
end

for i=1:size(Time_MPI,1)
  Legend_vector{i} =['MPI-Tasks ',num2str(2^i)];
end
Time_MPI

for i=1:6
  A(i,:) = Q_Vector +i -1;
end

h = figure(1);
hold on
save('Serial_Mpi_Time','Time_MPI')


plot (A(1,:),Time_MPI(1,:),'b')
plot (A(2,:),Time_MPI(2,:),'g')
plot (A(3,:),Time_MPI(3,:),'r')
plot (A(4,:),Time_MPI(4,:),'c')
plot (A(5,:),Time_MPI(5,:),'m')
plot (A(6,:),Time_MPI(6,:),'k')


set(gca, 'XTick',1:9, 'XTickLabel',[17, 18, 19 ,20 ,21 ,22 ,23, 25, 25, 26])
xlabel('N+Q')
ylabel('serial/parallel')
legend(Legend_vector,'Location','Best')

grid
if((size(name))>1)
  saveas(h,'Diagramm.jpeg')
else
  title(name)
  saveas(h,[name '.png'])
end
