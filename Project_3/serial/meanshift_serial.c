#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <float.h>
#include <math.h>
struct timeval startwtime, endwtime;
double seq_time;


int N; // number of elements
int D; // dimensions
int sigma = 1;
double MS_error = 0.0001; // mean shift error


double **x; // initilal matrix
double **y; // final matrix
double **y_new;
double **val_y; // validation final matrix
double **m; // mean shift vectrors
double **g; // gaussian
double **numer; // numerator
double **denom; // denominator

void init(void);
void mean_shift(void);
void test(void);

int main(int argc, char **argv) {


  if (argc != 3) {
    printf("Usage: %s N data \n D dimension  )\n",
           argv[0]);
    exit(1);
  }
  


  N =  atoi(argv[1]);
  D =  atoi(argv[2]);
  
  
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
  double temp;
  FILE *f;

  x = (double **) malloc(N * sizeof(double*));
  for (i = 0; i < N; i++) {
    x[i] = (double *) malloc(D * sizeof(double));
  }

  y = (double **) malloc(N * sizeof(double*));
  for (i = 0; i < N; i++) {
    y[i] = (double *) malloc(D * sizeof(double));
  }
  
  y_new = (double **) malloc(N * sizeof(double*));
  for (i = 0; i < N; i++) {
    y_new[i] = (double *) malloc(D * sizeof(double));
  }
  
  val_y = (double **) malloc(N * sizeof(double*));
  for (i = 0; i < N; i++) {
    val_y[i] = (double *) malloc(D * sizeof(double));
  }

  m = (double **) malloc(N * sizeof(double*));
  for (i = 0; i < N; i++) {
    m[i] = (double *) malloc(D * sizeof(double));
  }


  g = (double **) malloc(N * sizeof(double*));
  for (i = 0; i < N; i++) {
    g[i] = (double *) malloc(N * sizeof(double));
  }

  numer = (double **) malloc(N * sizeof(double*));
  for (i = 0; i < N; i++) {
    numer[i] = (double *) malloc(D * sizeof(double));
  }
  denom = (double **) malloc(N * sizeof(double*));
  for (i = 0; i < N; i++) {
    denom[i] = (double *) malloc(D * sizeof(double));
  }

  f = fopen("../data/r15.txt","r");
  for (i = 0; i < N; ++i) {
    for (j = 0; j < D; ++j) {
      ret_code = fscanf(f, "%lf\t", &x[i][j]);
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
      m[i][j] = DBL_MAX;
    }
  }


}





void mean_shift() {
  int iter = 0;
  int i,j,z;
  double dist = 0;
  double er = DBL_MAX;
  double temp;
  while(er > MS_error) {
     
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

      temp = sqrt(temp);
      er += temp;

    }
 
    er = sqrt(er);

    for(i=0;i<N;i++){
      for(z=0;z<D;z++){
        y[i][z] = y_new[i][z];
        numer[i][z] = 0;
        denom[i][z] = 0;
      }

    }
   //printf("%lf,,,,,,%lf\n",y_new[1][1],y[1][1]); 

    printf("Iteration = %d, Error = %lf\n",iter,er);

  } 
  



}




void test() {
    int i,j;
    int pass = 1;
    int ret_code = 0;
    int count = 0;
    double error = 0.0001; 
    FILE *f;
    f = fopen("../data/validation_r15.txt","r");
    for (i = 0; i < N; ++i) {
      for (j = 0; j < D; ++j) {
        ret_code = fscanf(f, "%lf\t", &val_y[i][j]);
      }
        ret_code = fscanf(f,"\n");
    }
    fclose(f);
    printf("%lf\n",fabs(val_y[1][1]-y[1][1]));
    
   f = fopen("yout.txt","w+");
   for (i = 0; i < N; i++) {
      for (j = 0; j < D; j++) {
        fprintf(f,"%lf\t",y[i][j]);
      }
      fprintf(f,"\n");
    }
    fclose(f);



    for (i = 0; i < N; ++i) {
      for (j = 0; j < D; ++j) {

	//pass &= (abs(val_y[i][j] - y[i][j]) <= error);
        
	if(fabs(val_y[i][j] - y[i][j]) > error){
	  count++;
          pass=0;
        }
      }
    }
    printf(" TEST %s\n",(pass) ? "PASSed" : "FAILed");
    printf("%d\n",count);



}

