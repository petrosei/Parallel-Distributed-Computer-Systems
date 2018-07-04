/*

PageRank 

Gause-Seidel

*/


#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <string.h>
#include <omp.h>

struct timeval startwtime, endwtime;
double seq_time;

//Global variables
float c;
float tol;
int N;
float v;
float b;
int maxiter;
int NT; //Number of threads
//Global Arrays

//float *v; //the personalization vector of length N
float **A; //the adjacency matrix
float **AT; //Translosed A matrix
float **P; // P = (I-c*At)
float *x; // pagerank value
float *xold; // old pagerank
int *out_link;

char Dset[20];




void init(){

  int i,j;
  
  FILE *fnodes;
  FILE *flist;
  FILE *fmatrix;

  char nodes_file[1000];
  char  list_file[1000];
  char  matrix_file[1000];

  char path[40];

   bzero(path,40);
   strcpy(path,"../Datasets/");
   strcat(path,Dset);

  c = 0.85;
  maxiter = 500;
  tol = 0.0000001;




/*** open the nodes file to obtain the number of nodes ***/
 
 sprintf(nodes_file,"%s/graph/nodes",path);
 fnodes = fopen(nodes_file,"r");
 if (fnodes == NULL){
   printf("ERROR: Cant open file %s\n",nodes_file);
   exit(1);
 }
 fscanf(fnodes,"%d",&N);

 fclose(fnodes);


  v = 1/((float)N);

  b = (1-c)*v;
  //printf("%f\n",v);
  // Initialize x vector x = v
  
  x = (float *)malloc(N*sizeof(float));
  for (i=0;i<N;i++){
    x[i] = v;
  }

  xold = (float *)malloc(N*sizeof(float));
  for (i=0;i<N;i++){
    xold[i] = v;
  }

/**** Read List and Construct the adjacency matrix A ****/

 A = (float **)malloc(N*sizeof(float *));
 for (i = 0; i < N; i ++){
   A[i] = (float *)malloc(N*sizeof(float));
   for (j = 0; j < N; j ++){
     A[i][j] = 0;
   }
 }

 sprintf(list_file,"%s/graph/adj_list",path);
 flist = fopen(list_file,"r");
 for (i = 0; i < N; i ++){
   fscanf(flist,"%*d: %d",&j);
   while (j != -1){
     A[i][j] = 1;
     fscanf(flist,"%d",&j);
   }
 }

 fclose(flist);
  


/*** print the adjacency matrix ***/

/*

 //sprintf(matrix_file,"%s/graph/adj_matrix",path);
 //fmatrix = fopen(matrix_file,"w");
  for (i = 0; i < N; i ++){
    printf("%d: ", i);
    for (j = 0; j < N; j ++){
      if (A[i][j]==1)
      printf("%f ", A[i][j]);
    }
    printf("\n");
  }
 //fclose(fmatrix);

*/

  // Initialize the output link vector
  out_link = (int *)malloc(N*sizeof(int));

  for (i=0; i<N; i++) {
    out_link[i] = 0;
  }

  // Manage dangling nodes   
  for (i=0; i<N; i++) {
    for (j=0; j<N; j++) {
      if (A[i][j] != 0.0) {
        out_link[i] = out_link[i] + 1;
      }
    }
  }




/*********************** MATRIX STOCHASTIC-FIED  ***********************/

  for (i=0; i<N; i++){
    if (out_link[i] == 0){
      // Deal with dangling nodes
      for (j=0; j<N; j++){
        A[i][j] = 1.0 / N;
      }
    } 
    else {
        for (j=0; j<N; j++){
          if (A[i][j] != 0) {
	    A[i][j] = A[i][j] / out_link[i];
          }
	}
    }
  }



  /**** Create transposed matrix A ****/

  AT = (float **)malloc(N*sizeof(float *));

  for (i = 0; i < N; i ++){
    AT[i] = (float *)malloc(N*sizeof(float));
    for (j = 0; j < N; j ++){
      AT[i][j] = 0;
    }
  }




  // Transpose the matrix 
  for (i=0; i<N; i++){
    for (j=0; j<N; j++){
      AT[j][i] = A[i][j];
    }
  }


  //*** Create of matrix P=(I-c*AT)

  P = (float **)malloc(N*sizeof(float *));

  for (i = 0; i < N; i ++){
    P[i] = (float *)malloc(N*sizeof(float));
    for (j = 0; j < N; j ++){
      P[i][j] = 0;
    }
  }

  // P matrix  
  for (i=0; i<N; i++){
    for (j=0; j<N; j++){
      P[i][j] = -c*AT[i][j];
      if(i==j){
        P[i][j] = 1 + P[i][j];
      }
    }
  }




} // End of init() function



void gauseSeidel(){

  int i, j;
  float xnew;
  
  int chunk =N/4;
  #pragma omp parallel num_threads(NT) shared(P,x,chunk)  private(i,j,xnew)
  {
    #pragma omp for schedule(dynamic,chunk) 
  for (i=0;i<N;i++){
    xnew = b;

  
    for(j=0;j<N;j++){
      if (i != j){
        xnew -= P[i][j]*x[j];
      }  
    }
  
    x[i] = xnew/P[i][i];
  }
}



} // End of gauseSeidel() function


void pageRank(){

  int i, iter;
  float error;
  iter = 0;
  error = 1;
 
  while (error>tol && iter<maxiter){
 
  for (i=0;i<N;i++){
    xold[i] = x[i];
  }  

  gauseSeidel();
  
  iter++;
  error = 0;
  for (i=0;i<N;i++){
    error += (x[i]-xold[i])*(x[i]-xold[i]);
  }

  error = sqrt(error); 
  printf("iter=%d, error=%f\n",iter,error);
  }  


}// End of pageRank() function





/** the main program **/ 
int main(int argc, char **argv) {

  if (argc != 3) {
    printf("Usage:\n 1. Dset: Dataset folder (You need to place it inside Datasets folder) \n 2. NT: Number of threads\n ");
    exit(1);
  }

  NT = atoi(argv[2]);
  
  bzero(Dset,20);
  strcpy(Dset,argv[1]);

  int i;
  init();  

  gettimeofday (&startwtime, NULL);
  pageRank();
  gettimeofday (&endwtime, NULL);

  for (i=0;i<N;i++){
    printf("%f ",x[i]);
  }
  printf("\n");
  seq_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6
		      + endwtime.tv_sec - startwtime.tv_sec);

  printf("Serial wall clock time = %f\n", seq_time);

  


}




