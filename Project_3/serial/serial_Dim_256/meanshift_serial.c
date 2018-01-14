#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <float.h>
#include <math.h>
struct timeval startwtime, endwtime;
double seq_time;


int N = 256; // number of elements
int D = 64; // dimensions
int sigma = 10;
float MS_error = 0.0001; // mean shift error
float error = 127 * 0.001; // mean value of dataset * 10^(-3) 
int iter_limit = 4;

float **x; // initilal matrix
float **y; // final matrix
float **y_new;
float **val_y; // validation final matrix
float **m; // mean shift vectrors
float **g; // gaussian
float **numer; // numerator
float **denom; // denominator

void init(void);
void mean_shift(void);
void test(void);

int main(int argc, char **argv) {


  MS_error = MS_error*sigma; 
  
  init();

  gettimeofday (&startwtime, NULL);
  
  mean_shift();

  gettimeofday (&endwtime, NULL);
  
  seq_time = (long double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6 + endwtime.tv_sec - startwtime.tv_sec);

  printf("KNN wall clock time = %f\n", seq_time);
  

  test();
}




/** procedure init() : initialize array "a" with data **/
void init() {
  int i,j;
  int ret_code = 0;
  float temp;
  FILE *f;


  // Allocate system memmory

  x = (float **) malloc(N * sizeof(float*));
  for (i = 0; i < N; i++) {
    x[i] = (float *) malloc(D * sizeof(float));
  }

  y = (float **) malloc(N * sizeof(float*));
  for (i = 0; i < N; i++) {
    y[i] = (float *) malloc(D * sizeof(float));
  }
  
  y_new = (float **) malloc(N * sizeof(float*));
  for (i = 0; i < N; i++) {
    y_new[i] = (float *) malloc(D * sizeof(float));
  }
  
  val_y = (float **) malloc(N * sizeof(float*));
  for (i = 0; i < N; i++) {
    val_y[i] = (float *) malloc(D * sizeof(float));
  }

  m = (float **) malloc(N * sizeof(float*));
  for (i = 0; i < N; i++) {
    m[i] = (float *) malloc(D * sizeof(float));
  }


  g = (float **) malloc(N * sizeof(float*));
  for (i = 0; i < N; i++) {
    g[i] = (float *) malloc(N * sizeof(float));
  }

  numer = (float **) malloc(N * sizeof(float*));
  for (i = 0; i < N; i++) {
    numer[i] = (float *) malloc(D * sizeof(float));
  }
  denom = (float **) malloc(N * sizeof(float*));
  for (i = 0; i < N; i++) {
    denom[i] = (float *) malloc(D * sizeof(float));
  }

  f = fopen("../../data/Dim/Dim_256x64.txt","r");
  for (i = 0; i < N; ++i) {
    for (j = 0; j < D; ++j) {
      ret_code = fscanf(f, "%f\t", &x[i][j]);
    }
    ret_code = fscanf(f,"\n");   
  }
  fclose(f);

  for (i = 0; i < N; ++i) {
    for (j = 0; j < D; ++j) {
      y[i][j] = x[i][j];
    }
  }
  
  for (i = 0; i < N; ++i) {
    for (j = 0; j < D; ++j) {
      m[i][j] = FLT_MAX;
    }
  }


}





void mean_shift() {
  int iter = 0;
  int i,j,z;
  float dist = 0;
  float er = FLT_MAX;
  float last_er = FLT_MAX;
  float temp;
  while(er > MS_error && iter<iter_limit && last_er>=er) {
    last_er = er;
    iter++;
    er = 0;
    for(i=0;i<N;i++){
      
      for(j=0;j<N;j++){

        for(z=0;z<D;z++){

          dist += pow((y[i][z]-x[j][z]),2);

        }
        dist = sqrt(dist);

        if (dist>pow(sigma,2)) g[i][j]=0;
        else g[i][j] = exp(-dist/(2*pow(sigma,2)));

        if (i==j) g[i][j] += 1;
        dist=0;

        for(z=0;z<D;z++){
          numer[i][z] += g[i][j]*x[j][z]; 
          denom[i][z] +=g[i][j];
        }

      }

      temp = 0;
      for(z=0;z<D;z++){
        y_new[i][z] = numer[i][z]/denom[i][z];
        m[i][z] = y_new[i][z] - y[i][z];
        temp += pow(m[i][z],2); 
      }

      
      er += temp;
      
      
    
      for(z=0;z<D;z++){
        y[i][z] = y_new[i][z];
        numer[i][z] = 0;
        denom[i][z] = 0;
      }

    
    }
 
    er = sqrt(er);


    printf("Iteration = %d, Error = %f\n",iter,er);

  } 
  



}




void test() {
    int i,j;
    int pass = 1;
    int ret_code = 0;
    int count = 0; 
    FILE *f;
    f = fopen("../../data/Dim/validation_Dim_256x64.txt","r");
    for (i = 0; i < N; ++i) {
      for (j = 0; j < D; ++j) {
        ret_code = fscanf(f, "%f\t", &val_y[i][j]);
      }
        ret_code = fscanf(f,"\n");
    }
    fclose(f);
    
   f = fopen("yout.txt","w+");
   for (i = 0; i < N; i++) {
      for (j = 0; j < D; j++) {
        fprintf(f,"%f\t",y[i][j]);
      }
      fprintf(f,"\n");
    }
    fclose(f);



    for (i = 0; i < N; ++i) {
      for (j = 0; j < D; ++j) {

        
	if(fabs(val_y[i][j] - y[i][j]) > error){
	  count++;
          pass=0;
        }
      }
    }
    printf(" TEST %s\n",(pass) ? "PASSed" : "FAILed");
    printf(" Errors = %d\n",count);



}

