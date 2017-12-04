#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <float.h>
#include <mpi.h>
#include <string.h>

struct timeval startwtime, endwtime;
double seq_time;


//Arguments
int K;
int N;
int D;
int choice;
int chunk;
//MPI variables
int world_size;
int proc_rank;



//Matrices
double **Nstart;
double **Ncomp;
double **Nbuf;

double **KNN_dist;
double **val_dist;
int **KNN_id;

//Functions
void init(void);
void knn(void);
void test(void);

int main(int argc, char **argv) {

  if (argc != 4 && argc !=5) {
    printf("Usage: %s k nearest neighbours \n N data \n D dimension \n choice to create dataset )\n",
           argv[0]);
    exit(1);
  }
  
  if (argc==5) choice = 1;
  else choice = 0;


  K = atoi(argv[1]);
  N =  atoi(argv[2]);
  D =  atoi(argv[3]);
   
  init();
  

  MPI_Barrier(MPI_COMM_WORLD);

  if (!proc_rank) gettimeofday (&startwtime, NULL);
  
  knn();

  MPI_Barrier(MPI_COMM_WORLD);
  if (!proc_rank) { 
    gettimeofday (&endwtime, NULL);
  
    seq_time = (long double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6 + endwtime.tv_sec - startwtime.tv_sec);

    printf("KNN wall clock time = %f\n", seq_time);
  }

  test();
  
  // Finalize the MPI environment.
  MPI_Finalize();

}




/** procedure init() : initialize array "a" with data **/
void init() {
  int i,j;
  int ret_code = 0;
  double temp;
  double null;
  FILE *f;
  

  MPI_Init(NULL, NULL);

  // Get the number of processes
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  // Get the rank of the process
  MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);

  // Print off a hello world message
  printf("Hello world from id %d out of %d processors\n", proc_rank, world_size);
  
  chunk = N/world_size;
 
  Nstart = (double **) malloc(chunk * sizeof(double*));
  for (i = 0; i < chunk; i++) {
    Nstart[i] = (double *) malloc(D * sizeof(double));
  }

  Ncomp = (double **) malloc(chunk * sizeof(double*));
  for (i = 0; i < chunk; i++) {
    Ncomp[i] = (double *) malloc(D * sizeof(double));
  }
  
  Nbuf = (double **) malloc(chunk * sizeof(double*));
  for (i = 0; i < chunk; i++) {
    Nbuf[i] = (double *) malloc(D * sizeof(double));
  }

  KNN_dist = (double **) malloc(chunk * sizeof(double*));
  for (i = 0; i < chunk; i++) {
    KNN_dist[i] = (double *) malloc(K * sizeof(double));
  }
  
   val_dist = (double **) malloc(chunk * sizeof(double*));
  for (i = 0; i < N; i++) {
    val_dist[i] = (double *) malloc(K * sizeof(double));
  } 

  KNN_id = (int **) malloc(chunk * sizeof(int*));
  for (i = 0; i < chunk; i++) {
    KNN_id[i] = (int *) malloc(K * sizeof(int));
  }

  
  for (i = 0; i < chunk; i++) {
    for (j = 0; j < K; j++) {
      KNN_dist[i][j] = DBL_MAX;
    }
  }

  if (choice) {
    f = fopen("corpus.txt","w+");
    for (i = proc_rank*chunk; i < (proc_rank+1)*chunk; i++) {
      for (j = 0; j < D; j++) {
        temp = (double)rand()/(double)(RAND_MAX) * 100.0; // (N - i);
        fprintf(f,"%lf\t",temp);
      }
      fprintf(f,"\n");
    }
    fclose(f);
  }





// Read a chunk of corpus from enery process 
    

    f = fopen("corpus.txt","r");
    
    for (i = 0; i < N; i++) {
      for (j = 0; j < D; j++) {
        
	if (proc_rank==(i/chunk)) {
          ret_code = fscanf(f, "%lf\t", &Nstart[i%chunk][j]);
          Ncomp[i%chunk][j] = Nstart[i%chunk][j];
          Nbuf[i%chunk][j] = Nstart[i%chunk][j];

          //if (proc_rank == 2 && (i%chunk)<10) printf("%lf\t",Nstart[i%chunk][j]);
        }
        else
          ret_code = fscanf(f, "%lf\t",&null);
      }
	ret_code = fscanf(f,"\n");
        //if (proc_rank == 2 && (i%chunk)<10) printf("\n");   
    }   
    fclose(f);

}





void knn() {
  int w,i,j,z,l,id,tempid,next,prev; 
  double dist = 0,tempdist;
     
  MPI_Status stat;

  next = proc_rank + 1;
  prev = proc_rank - 1;
  if (next == world_size) next = 0;
  if (prev < 0) prev = world_size-1;
  for (w=0; w < world_size; w++) { 



    if(proc_rank%2==0){
      MPI_Recv(&Nbuf[0][0],chunk*(D+2),MPI_DOUBLE,prev,1,MPI_COMM_WORLD,&stat); 
      MPI_Ssend(&Ncomp[0][0],chunk*(D+2),MPI_DOUBLE,next,1,MPI_COMM_WORLD);
    }
    else {
      
      MPI_Send(&Ncomp[0][0],chunk*(D+2),MPI_DOUBLE,next,1,MPI_COMM_WORLD);
      MPI_Recv(&Nbuf[0][0],chunk*(D+2),MPI_DOUBLE,prev,1,MPI_COMM_WORLD,&stat); 
    }

    for (z = 0; z < chunk;z++) {
      
      for (i = 0; i < chunk; i++) {
        
        if (i==z && !w) continue;
         
        for (j = 0; j < D; j++) {
          dist += (Nstart[z][j]-Ncomp[i][j])*(Nstart[z][j]-Ncomp[i][j]);
        }
        
 
        if (dist<=KNN_dist[z][K-1]) {
       	
          id = i; 
	  for (l = 0; l < K; l++) {
            
            if (dist<KNN_dist[z][l]){
	      tempdist = KNN_dist[z][l];
              tempid = KNN_id[z][l];
              KNN_dist[z][l] = dist;
	      KNN_id[z][l] = id;
              dist = tempdist;
              id = tempid;
            
            }

          }

        }
        dist = 0;

     }

   }

  
  
  for (i = 0; i < chunk; i++) {
    for (j = 0; j < D; j++) {
    
    Ncomp[i][j] = Nbuf[i][j];

    }
  }

  
  }

}

void test() {
    int i,j;
    int pass = 1;
    int count = 0;
    int ret_code = 0;
    char* name;
    double error = 0.000001;
    double null,temp;
    FILE *f;
    
    f = fopen("../validation/validate.txt","r");
  
    for (i = 0; i < N; ++i) {
      for (j = 0; j < K; ++j) {

        if (proc_rank==(i/chunk)) {

          ret_code = fscanf(f, "%lf\t", &val_dist[i%chunk][j]);

         }
        else    
          ret_code = fscanf(f, "%lf\t",&null);
      }
        ret_code = fscanf(f,"\n");
    }
    fclose(f);

   MPI_Barrier(MPI_COMM_WORLD);

   name = (char *) malloc(13 * sizeof(char));

   name[0] = 'K';
   name[1] = 'm';
   name[2] = 'a';
   name[3] = 't';
   name[4] = 'r';
   name[5] = 'i';
   name[6] = 'x';
   name[7] = (char) (proc_rank+48);
   name[8] = '.';
   name[9] = 't';
   name[10] = 'x';
   name[11] = 't';
   name[12] = '\0';

   //printf("%s\n",name);

   f = fopen(name,"w+");
   for (i = 0; i < chunk; i++) {
      for (j = 0; j < K; j++) {
        fprintf(f,"%lf\t",KNN_dist[i][j]);
      }
      fprintf(f,"\n");
    }
    fclose(f);

    for (i = 0; i < chunk; ++i) {
      for (j = 0; j < K; ++j) {

	//pass &= ((val_dist[i][j] - KNN_dist[i][j]) <= error);
	if((val_dist[i][j] - KNN_dist[i][j]) >= error){
	   pass = 0;
           //count++;
           //null  =  KNN_dist[i][j];
           //temp = val_dist[i][j];
          //printf("%d\t%d\t%lf\n",i,j,val_dist[i][j]); 
        }
      }
    }
    printf("%s\t TEST %s\n",name,(pass) ? "PASSed" : "FAILed");
    //printf("%d\n",count);
    //printf("%lf\n\n\n\n",val_dist[0][0]);



}

