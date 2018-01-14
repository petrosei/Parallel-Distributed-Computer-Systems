#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <float.h>
#include <mpi.h>
#include <string.h>
#include <math.h>
struct timeval startwtime, endwtime;
double seq_time;


//Arguments
int K = 30;
int N = 60000;
int D = 30;
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

   
  init();
  

  MPI_Barrier(MPI_COMM_WORLD);

  if (!proc_rank) gettimeofday (&startwtime, NULL);
  
  MPI_Barrier(MPI_COMM_WORLD);
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




// Read a chunk of corpus from enery process 
    

    f = fopen("../../data/mnist_train_svd/mnist_train_svd.txt","r");
    
    for (i = 0; i < N; i++) {
      for (j = 0; j < D; j++) {
        
	if (proc_rank==(i/chunk)) {
          ret_code = fscanf(f, "%lf\t", &Nstart[i%chunk][j]);
          Ncomp[i%chunk][j] = Nstart[i%chunk][j];
          Nbuf[i%chunk][j] = Nstart[i%chunk][j];

        }
        else
          ret_code = fscanf(f, "%lf\t",&null);
      }
	ret_code = fscanf(f,"\n");
    }   
    fclose(f);

}





void knn() {
  int w,i,j,z,l,id,tempid,next,prev; 
  double dist = 0,tempdist;
  MPI_Request reqs[2];
  MPI_Status stats[2]; 


  next = proc_rank + 1;
  prev = proc_rank - 1;
  if (next == world_size) next = 0;
  if (prev < 0) prev = world_size-1;
  for (w=0; w < world_size; w++) { 


    MPI_Isend(&Ncomp[0][0],chunk*(D+2),MPI_DOUBLE,next,proc_rank,MPI_COMM_WORLD,&reqs[0]);
    MPI_Irecv(&Nbuf[0][0],chunk*(D+2),MPI_DOUBLE,prev,prev,MPI_COMM_WORLD,&reqs[1]); 

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

  
  
  MPI_Waitall(2, reqs, stats);
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
    double error = 0.00001;
    double null,temp;
    FILE *f;
    
    f = fopen("../../data/mnist_train_svd/validation_mnist_train_svd.txt","r");
  
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

	if(abs(val_dist[i][j] - KNN_dist[i][j]) >= error){
	   pass = 0;
           count++;
        }
      }
    }
    printf("%s\t TEST %s\n",name,(pass) ? "PASSed" : "FAILed");
    printf("%d\n",count);



}

