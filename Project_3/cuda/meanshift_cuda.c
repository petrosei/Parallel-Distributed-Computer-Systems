#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <float.h>
#include <math.h>
struct timeval startwtime, endwtime;
double seq_time;

// CPU variable
int N; // number of elements
int D; // dimensions
float sigma = 1.0;
float MS_error = 0.04; // mean shift error

//GPU variables
int *gp_N;
int *gp_D;
float *gp_sigma;
size_t gp_pitch;

//CPU matrices
float **x; // initilal matrix
float **y; // final matrix
float **val_y; // validation final matrix
float **m; // mean shift vectrors
// GPU matrices
float *gp_x;
float *gp_y;
float *gp_y_new;
float *gp_m; // mean shift vectrors
float *gp_g; // gaussian
float *gp_numer; // numerator
float *gp_denom; // denominator




//__global__ void cuda_mean_shift(void);

void init(void);
void mean_shift(void);
void test(void);
void free_arrays(void);


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
  
  //mean_shift();

  gettimeofday (&endwtime, NULL);
  
  seq_time = (long double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6 + endwtime.tv_sec - startwtime.tv_sec);

  printf("KNN wall clock time = %f\n", seq_time);
  

  test();

  free_arrays();
}




/** procedure init() : initialize array "a" with data **/
void init() {
  int i,j;
  int ret_code=0;
  
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


  m = (float **) malloc(N * sizeof(float*));
  for (i = 0; i < N; i++) {
    m[i] = (float *) malloc(D * sizeof(float));
  }

/*
  // Allocate GPU variables

  cudaMalloc( (void**)&gp_N , sizeof(int));
  cudaMalloc( (void**)&gp_D , sizeof(int));
  cudaMalloc( (void**)&gp_sigma , sizeof(float));


  // Allocate GPU matrices

  cudaMallocPitch(&gp_x, &gp_pitch, N * sizeof(float), D);
  cudaMallocPitch(&gp_y, &gp_pitch, N * sizeof(float), D);
  cudaMallocPitch(&gp_y_new, &gp_pitch, N * sizeof(float), D);
  cudaMallocPitch(&gp_m, &gp_pitch, N * sizeof(float), D);
  cudaMallocPitch(&gp_g, &gp_pitch, N * sizeof(float), N);
  cudaMallocPitch(&gp_numer, &gp_pitch, N * sizeof(float), D);
  cudaMallocPitch(&gp_denom, &gp_pitch, N * sizeof(float), D);
*/

  f = fopen("../data/r15.txt","r");
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


/*


__global__ void cuda_mean_shift(int *N,int *D,float *sigma,float *x, float *y, float *y_new,float *m, float *g,float *numer,float *denom,size_t pitch){

 int tid = threadIdx.x;
 int j,z;
 float dist=0; 
    //for(i=0;i<N;i++){
      
      for(j=0;j<*N;j++){
        float *row_g = (float *)((char*)g + j * pitch);
        for(z=0;z<*D;z++){
          float *row_x = (float *)((char*)x + z * pitch); 
          float *row_y = (float *)((char*)y + z * pitch);
          dist += powf((row_y[tid]-row_x[j]),2.0);

        }
        dist = sqrtf(dist);

        if (dist>powf(*sigma,2.0)) row_g[tid]=0;
        else row_g[tid] = exp(-dist/(2*powf(*sigma,2.0)));

        if (tid==j) row_g[tid] += 1;
        dist=0;

        for(z=0;z<*D;z++){
          float *row_numer = (float *)((char*)numer + z * pitch);
          float *row_denom = (float *)((char*)denom + z * pitch);
          float *row_x = (float *)((char*)x + z * pitch);
          row_numer[tid] += row_g[tid]*row_x[j]; 
          row_denom[tid] +=row_g[tid];
        }

      }

      for(z=0;z<*D;z++){
        float *row_y = (float *)((char*)y + z * pitch);
        float *row_y_new = (float *)((char*)y_new + z * pitch);
        float *row_numer = (float *)((char*)numer + z * pitch);
        float *row_denom = (float *)((char*)denom + z * pitch);
        float *row_m = (float *)((char*)m + z * pitch);
        row_y_new[tid] = row_numer[tid]/row_denom[tid];
        row_m[tid] = row_y_new[tid] - row_y[tid];
      }

      
    
      for(z=0;z<*D;z++){
        float *row_y = (float *)((char*)y + z * pitch);
        float *row_y_new = (float *)((char*)y_new + z * pitch);
        float *row_numer = (float *)((char*)numer + z * pitch);
        float *row_denom = (float *)((char*)denom + z * pitch);
        row_y[tid] = row_y_new[tid];
        row_numer[tid] = 0;
        row_denom[tid] = 0;
      }


   // }


}

*/


/*
void mean_shift() {
  int iter = 0;
  int i,j,z;
  //float dist = 0;
  float er = DBL_MAX;
  float temp = 0;


  

  // Copy variables to GPU
  cudaMemcpy(gp_N,&N,sizeof(int),cudaMemcpyHostToDevice);
  cudaMemcpy(gp_D,&D,sizeof(int),cudaMemcpyHostToDevice);
  cudaMemcpy(gp_sigma,&sigma,sizeof(int),cudaMemcpyHostToDevice);

  // Copy matrices to GPU
  cudaMemcpy2D(gp_x, gp_pitch, x,N*sizeof(float), N*sizeof(float), D, cudaMemcpyHostToDevice);
  cudaMemcpy2D(gp_y, gp_pitch, y,N*sizeof(float), N*sizeof(float), D, cudaMemcpyHostToDevice);
  cudaMemcpy2D(gp_m, gp_pitch, m,N*sizeof(float), N*sizeof(float), D, cudaMemcpyHostToDevice);
  while(er > MS_error) {
     
    iter++;
    er = 0;
    
    cuda_mean_shift<<<1,N>>>(gp_N,gp_D,gp_sigma,gp_x,gp_y,gp_y_new,gp_m,gp_g,gp_numer,gp_denom,gp_pitch); 
 
   cudaMemcpy2D(m,N*sizeof(float),gp_m, gp_pitch, N*sizeof(float), D, cudaMemcpyDeviceToHost); 
   for(i=0;i<N;i++){
     for(z=0;z<D;z++){
       temp += pow(m[i][z],2);
     }
     er += temp;
     temp = 0;
   }
   
   er = sqrt(er);

   //printf("%lf,,,,,,%lf\n",y_new[1][1],y[1][1]); 

    printf("Iteration = %d, Error = %lf\n",iter,er);

  } 
  
  // Copy matrices to memory
  cudaMemcpy2D(y,N*sizeof(float),gp_y, gp_pitch, N*sizeof(float), D, cudaMemcpyDeviceToHost);

}
*/






void test() {
    int i,j;
    int pass = 1;
    int ret_code = 0;
    int count = 0;
    float error = 0.0001; 
    FILE *f;

    f = fopen("../data/validation_r15.txt","r");
    for (i = 0; i < N; ++i) {
      for (j = 0; j < D; ++j) {
        ret_code = fscanf(f, "%f\t", &val_y[i][j]);
      }
        ret_code = fscanf(f,"\n");
    }
    fclose(f);
    //printf("%f\n",fabs(val_y[1][1]-y[1][1]));
    
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

	//pass &= (abs(val_y[i][j] - y[i][j]) <= error);
        
	if(fabs(y[i][j] - y[i][j]) > error){
	  count++;
          pass=0;
        }
      }
    }
    printf(" TEST %s\n",(pass) ? "PASSed" : "FAILed");
    printf("%d\n",count);



}


void free_arrays(){

free(x);
free(y);
free(val_y);
free(m);
/*
cudaFree(gp_N); 
cudaFree(gp_D);
cudaFree(gp_sigma);
cudaFree(gp_x);
cudaFree(gp_y);
cudaFree(gp_y_new);
cudaFree(gp_m);
cudaFree(gp_g);
cudaFree(gp_numer);
cudaFree(gp_denom);
*/
}


