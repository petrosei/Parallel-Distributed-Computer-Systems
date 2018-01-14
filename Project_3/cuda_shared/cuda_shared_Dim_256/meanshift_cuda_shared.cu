#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <float.h>
#include <math.h>
struct timeval startwtime, endwtime;
double seq_time;

// CPU variable
int N = 256; // number of elements
int D = 64; // dimensions
int nT = 64; // number of threads per block
float sigma = 10.0;
float MS_error = 0.0001; // mean shift error
float error = 127*0.1; // mean value of dataset * 10^(-3) 
int iter_limit = 4;
size_t cols;
size_t rows;

//GPU variables
int *gp_N;
int *gp_D;
float *gp_sigma;
size_t gp_pitch;

//CPU matrices
float *x; // initilal matrix
float *y; // final matrix
float **val_y; // validation final matrix
float *m; // mean shift vectrors
// GPU matrices
float *gp_x;
float *gp_y;
float *gp_y_new;
float *gp_m; // mean shift vectrors
float *gp_g; // gaussian
float *gp_numer; // numerator
float *gp_denom; // denominator




__global__ void cuda_mean_shift(void);

void init(void);
void mean_shift(void);
void test(void);
void free_arrays(void);


int main(int argc, char **argv) {

  MS_error = MS_error*sigma;  

  init();

  gettimeofday (&startwtime, NULL);
  
  mean_shift();

  gettimeofday (&endwtime, NULL);
  
  seq_time = (long double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6 + endwtime.tv_sec - startwtime.tv_sec);

  printf("KNN wall clock time = %f\n", seq_time);
  

  // Copy matrices to memory
  cudaMemcpy(y,gp_y, N*D*sizeof(float), cudaMemcpyDeviceToHost);
  test();

  free_arrays();
}




/** procedure init() : initialize array "a" with data **/
void init() {
  int i,j;
  int ret_code=0;
  
  FILE *f;

  // Allocate system memmory

  x = (float *) malloc(N*D * sizeof(float));
  y = (float *) malloc(N*D * sizeof(float));
  m = (float *) malloc(N*D * sizeof(float));


  val_y = (float **) malloc(N * sizeof(float*));
  for (i = 0; i < N; i++) {
    val_y[i] = (float *) malloc(D * sizeof(float));
  }
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


  f = fopen("../../data/Dim/Dim_256x64.txt","r");
  for (i = 0; i < N; ++i) {
    for (j = 0; j < D; ++j) {
      ret_code = fscanf(f, "%f\t", &x[i*D+j]);
    }
    ret_code = fscanf(f,"\n");   
  }
  fclose(f);

  for (i = 0; i < N; ++i) {
    for (j = 0; j < D; ++j) {
      y[i*D+j] = x[i*D+j];
    }
  }
  
  for (i = 0; i < N; ++i) {
    for (j = 0; j < D; ++j) {
      m[i*D+j] =FLT_MAX;
    }
  }

  // Copy variables to GPU
  cudaMemcpy(gp_N,&N,sizeof(int),cudaMemcpyHostToDevice);
  cudaMemcpy(gp_D,&D,sizeof(int),cudaMemcpyHostToDevice);
  cudaMemcpy(gp_sigma,&sigma,sizeof(int),cudaMemcpyHostToDevice);

  // Copy matrices to GPU
  cudaMemcpy(gp_x,x,N*D*sizeof(float), cudaMemcpyHostToDevice);
  cudaMemcpy(gp_y,y,N*D*sizeof(float), cudaMemcpyHostToDevice);
  cudaMemcpy(gp_m,m,N*D*sizeof(float), cudaMemcpyHostToDevice);

}





__global__ void cuda_mean_shift(int *N,int *D,float *sigma,float *x, float *y, float *y_new,float *m, float *g,float *numer,float *denom,size_t pitch){

 int tids = threadIdx.x;
 int tid = threadIdx.x + blockIdx.x * blockDim.x;
 int j,z;
 float r;
 float dist=0;
 __shared__ float s_y[64];
 __shared__ float s_m[64]; 
__shared__ float s_y_new[64];
__shared__ float s_numer[64];   
__shared__ float s_denom[64];
__shared__ float diff[64];

      s_y[tids] = y[tid];  
      s_m[tids] = m[tid];
      s_numer[tids] = 0;
      s_denom[tids] = 0;
    __syncthreads(); 
    if(tid<*N*(*D)){
      
      for(j=0;j<*N;j++){
        float *row_g = (float *)((char*)g + j * pitch);
          diff[tids] = powf((s_y[tids]-x[j*(*D)+tids]),2.0);
        __syncthreads();
        for(z=0;z<*D;z++){
          dist += diff[z];
        }
        
        dist = sqrtf(dist);

        if (dist>powf(*sigma,2.0)) row_g[(tid-tids)/blockDim.x]=0;
        else row_g[(tid-tids)/blockDim.x] = expf(-dist/(2*powf(*sigma,2.0)));

        if ((tid-tids)/blockDim.x == j) row_g[(tid-tids)/blockDim.x] += 1;
        dist=0;
          s_numer[tids] += row_g[(tid-tids)/blockDim.x]*x[j*(*D)+tids]; 
          s_denom[tids] +=row_g[(tid-tids)/blockDim.x];
        __syncthreads();

      }

        s_y_new[tids] = s_numer[tids]/s_denom[tids];
        s_m[tids] = s_y_new[tids] - s_y[tids];
    
        s_y[tids] = s_y_new[tids];
        s_numer[tids] = 0;
        s_denom[tids] = 0;
__syncthreads();

    }
    
      y[tid] = s_y[tids];
      m[tid] = s_m[tids];
    __syncthreads();


}





void mean_shift() {
  int iter = 0;
  int i,z;
  float er = FLT_MAX;
  float last_er = FLT_MAX;
  float temp = 0;


  

  while(er > MS_error && iter<iter_limit && last_er >= er) {
     
    last_er = er;
    iter++;
    er = 0;
    
    cuda_mean_shift<<<N,nT>>>(gp_N,gp_D,gp_sigma,gp_x,gp_y,gp_y_new,gp_m,gp_g,gp_numer,gp_denom,gp_pitch); 
   cudaMemcpy(m,gp_m,N*D*sizeof(float), cudaMemcpyDeviceToHost); 
   for(i=0;i<N;i++){
     for(z=0;z<D;z++){
       temp += pow(m[i*D+z],2);
     }
     er += temp;
     temp = 0;
   }
   
   er = sqrt(er);


    printf("Iteration = %d, Error = %lf\n",iter,er);

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
        fprintf(f,"%f\t",y[i*D+j]);
      }
      fprintf(f,"\n");
    }
    fclose(f);



    for (i = 0; i < N; ++i) {
      for (j = 0; j < D; ++j) {

        
	if(fabs(val_y[i][j] - y[i*D+j]) > error){
	  count++;
          pass=0;
        }
      }
    }
    printf(" TEST %s\n",(pass) ? "PASSed" : "FAILed");
    printf(" Errors = %d\n",count);



}


void free_arrays(){

free(x);
free(y);
free(val_y);
free(m);

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

}


