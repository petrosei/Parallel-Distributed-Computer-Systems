#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <float.h>
#include <math.h>
struct timeval startwtime, endwtime;
double seq_time;

int K;
int N;
int D;
int choice;
long double **a;

long double **KNN_dist;
long double **val_dist;
int **KNN_id;


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

  gettimeofday (&startwtime, NULL);
  
  knn();

  gettimeofday (&endwtime, NULL);
  
  seq_time = (long double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6 + endwtime.tv_sec - startwtime.tv_sec);

  printf("KNN wall clock time = %f\n", seq_time);
  

  test();
}




/** procedure init() : initialize array "a" with data **/
void init() {
  int i,j;
  int ret_code = 0;
  long double temp;
  FILE *f;

  a = (long double **) malloc(N * sizeof(long double*));
  for (i = 0; i < N; i++) {
    a[i] = (long double *) malloc(D * sizeof(long double));
  }
  KNN_dist = (long double **) malloc(N * sizeof(long double*));
  for (i = 0; i < N; i++) {
    KNN_dist[i] = (long double *) malloc(K * sizeof(long double));
  }
  
   val_dist = (long double **) malloc(N * sizeof(long double*));
  for (i = 0; i < N; i++) {
    val_dist[i] = (long double *) malloc(K * sizeof(long double));
  } 

  KNN_id = (int **) malloc(N * sizeof(int*));
  for (i = 0; i < N; i++) {
    KNN_id[i] = (int *) malloc(K * sizeof(int));
  }

  
  for (i = 0; i < N; i++) {
    for (j = 0; j < K; j++) {
      KNN_dist[i][j] = LDBL_MAX;
    }
  }
/*
  if (choice) {
    f = fopen("corpus.txt","w+");
    for (i = 0; i < N; i++) {
      for (j = 0; j < D; j++) {
        temp = (long double)rand()/(long double)(RAND_MAX) * 100.0; // (N - i);
        fprintf(f,"%Lf\t",temp);
      }
      fprintf(f,"\n");
    }
    fclose(f);
  }
 */ 
    f = fopen("mnist_train_svd.txt","r");
    for (i = 0; i < N; ++i) {
      for (j = 0; j < D; ++j) {
        ret_code = fscanf(f, "%Lf\t", &a[i][j]);
      }
	ret_code = fscanf(f,"\n");   
    }   
    fclose(f);
}



void knn() {
  int i,j,z,l,id,tempid; 
  long double dist,tempdist;
  
  for (z = 0; z < N;z++) {
    for (i = 0; i < N; i++) {
      if (i==z) continue;
      for (j = 0; j < D; j++) {
         dist += (a[z][j]-a[i][j])*(a[z][j]-a[i][j]);
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
}

void test() {
    int i,j;
    int pass = 1;
    int ret_code = 0;
    int count = 0;
    double error = 0.000001; 
    FILE *f;
    f = fopen("validation_mnist_train_svd.txt","r");
    for (i = 0; i < N; ++i) {
      for (j = 0; j < K; ++j) {
        ret_code = fscanf(f, "%Lf\t", &val_dist[i][j]);
      }
        ret_code = fscanf(f,"\n");
    }
    fclose(f);

    
   f = fopen("Kmatrix.txt","w+");
   for (i = 0; i < N; i++) {
      for (j = 0; j < K; j++) {
        fprintf(f,"%Lf\t",KNN_dist[i][j]);
      }
      fprintf(f,"\n");
    }
    fclose(f);



    for (i = 0; i < N; ++i) {
      for (j = 0; j < K; ++j) {

	pass &= (abs(val_dist[i][j] - KNN_dist[i][j]) <= error);
	//if((val_dist[i][j] - KNN_dist[i][j]) > error) count++;
      }
    }
    printf(" TEST %s\n",(pass) ? "PASSed" : "FAILed");
    //printf("%d\n",count);




}

