/*
 bitonic.c 

 This file contains two different implementations of the bitonic sort
        recursive  version :  recBitonicSort()
        imperative version :  impBitonicSort() 
 

 The bitonic sort is also known as Batcher Sort. 
 For a reference of the algorithm, see the article titled 
 Sorting networks and their applications by K. E. Batcher in 1968 


 The following codes take references to the codes avaiable at 

 http://www.cag.lcs.mit.edu/streamit/results/bitonic/code/c/bitonic.c

 http://www.tools-of-computing.com/tc/CS/Sorts/bitonic_sort.htm

 http://www.iti.fh-flensburg.de/lang/algorithmen/sortieren/bitonic/bitonicen.htm 
 */

/* 
------- ---------------------- 
   Nikos Pitsianis, Duke CS 
-----------------------------
*/


#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>
//#include <math.h>
struct timeval startwtime, endwtime;
double seq_time;


int N;          // data array size
int NT;		// number of Threads
int *a;         // data array to be sorted
int chunk;	// chunk in parallel region
int numoft = 1;
int tid0;
int tid1;
const int ASCENDING  = 1;
const int DESCENDING = 0;


void init(void);
void print(void);
void sort(void);
void test(void);
void exchange(int i, int j);
void compare(int i, int j, int dir);
void bitonicMerge(int lo, int cnt, int dir);
void recbitonicMerge(int lo, int cnt, int dir);
void recBitonicSort(int lo, int cnt, int dir);
void ser_recBitonicSort(int lo, int cnt, int dir);
void impBitonicSort(void);



/** the main program **/ 
int main(int argc, char **argv) {

int tid,nthreads;

  if (argc != 3) {
    printf("Usage: %s q\n  where n=2^q is problem size (power of two)\n", 
	   argv[0]);
    exit(1);
  }

  N = 1<<atoi(argv[1]);
  NT = atoi(argv[2]);
  a = (int *) malloc(N * sizeof(int));

  chunk = (N/NT);  
//omp_set_num_threads(1); 
omp_set_nested(1);

printf("Thread limit %d\n",omp_get_num_threads());
  init();

  gettimeofday (&startwtime, NULL);
  impBitonicSort();
  gettimeofday (&endwtime, NULL);

  seq_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6
		      + endwtime.tv_sec - startwtime.tv_sec);

  printf("Imperative wall clock time = %f\n", seq_time);

  test();


  init();
  gettimeofday (&startwtime, NULL);
  sort();
  printf("Number of threads %d\n",numoft);
  gettimeofday (&endwtime, NULL);

  seq_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6
		      + endwtime.tv_sec - startwtime.tv_sec);

  printf("Recursive wall clock time = %f\n", seq_time);

  test();

 #pragma omp parallel num_threads(NT)  private(tid)
{
  tid = omp_get_thread_num();
  //printf("Thread id = %d\n", tid);
  if (tid == 0) 
  {
    nthreads = omp_get_num_threads();
    //printf("Number of threads = %d\n", nthreads);
  }
}
  // print();
}

/** -------------- SUB-PROCEDURES  ----------------- **/ 

/** procedure test() : verify sort results **/
void test() {
  int pass = 1;
  int i;
  for (i = 1; i < N; i++) {
    pass &= (a[i-1] <= a[i]);
  }

  printf(" TEST %s\n",(pass) ? "PASSed" : "FAILed");
}


/** procedure init() : initialize array "a" with data **/
void init() {
  int i;
  for (i = 0; i < N; i++) {
    a[i] = rand() % N; // (N - i);
  }
}

/** procedure  print() : print array elements **/
void print() {
  int i;
  for (i = 0; i < N; i++) {
    printf("%d\n", a[i]);
  }
  printf("\n");
}


/** INLINE procedure exchange() : pair swap **/
inline void exchange(int i, int j) {
  int t;
  t = a[i];
  a[i] = a[j];
  a[j] = t;
}



/** procedure compare() 
   The parameter dir indicates the sorting direction, ASCENDING 
   or DESCENDING; if (a[i] > a[j]) agrees with the direction, 
   then a[i] and a[j] are interchanged.
**/
inline void compare(int i, int j, int dir) {
  if (dir==(a[i]>a[j])) 
    exchange(i,j);



}




/** Procedure bitonicMerge() 
   It recursively sorts a bitonic sequence in ascending order, 
   if dir = ASCENDING, and in descending order otherwise. 
   The sequence to be sorted starts at index position lo,
   the parameter cbt is the number of elements to be sorted. 
 **/
void bitonicMerge(int lo, int cnt, int dir) {
  if (cnt>1) {
    int k=cnt/2;
    int i;
    int ans;
    int tid;


    for (i=lo; i<lo+k; i++)
      compare(i, i+k, dir);


    	bitonicMerge(lo, k, dir);
	
    	bitonicMerge(lo+k, k, dir);
	
 }
}

/** Procedure recbitonicMerge() 
   It recursively sorts a bitonic sequence in ascending order, 
   if dir = ASCENDING, and in descending order otherwise. 
   The sequence to be sorted starts at index position lo,
   the parameter cbt is the number of elements to be sorted. 
 **/
void recbitonicMerge(int lo, int cnt, int dir) {
  if (cnt>1) {
    int k=cnt/2;
    int i;
    int ans;

    for (i=lo; i<lo+k; i++)
      compare(i, i+k, dir);
    recbitonicMerge(lo, k, dir);
    recbitonicMerge(lo+k, k, dir);
 
 }
}






/** function recBitonicSort() 
    first produces a bitonic sequence by recursively sorting 
    its two halves in opposite sorting orders, and then
    calls bitonicMerge to make them in the same order 
 **/
void recBitonicSort(int lo, int cnt, int dir) {
int k;

  if (cnt>1) {
      k=cnt/2;
//printf("Thread id = %d\n", krecB);
//printf("Thread id = %d\n", k);


//int nnn = omp_get_num_threads();
//    printf("Thread nnn = %d\n", nnn);

if( numoft+1>NT){
	ser_recBitonicSort(lo, k, ASCENDING);
	ser_recBitonicSort(lo+k, k, DESCENDING);
	//printf("Thread id \n");
	tid0 = omp_get_thread_num();
        printf("TID0 = %d\n",tid0);
}
else{

numoft = numoft +1;


#pragma omp parallel num_threads(2)  shared(a,k,lo,numoft) //if (omp_get_num_threads()< NT)
{


    #pragma omp single 
    {  

      
	#pragma omp task 
      {
	recBitonicSort(lo, k, ASCENDING);
	//tid0 = omp_get_thread_num();	
	//printf("TID0 = %d\n",tid0);
         
      }
      #pragma omp task 
      {
	//tid1 = omp_get_thread_num();
	recBitonicSort(lo+k, k, DESCENDING);
	 //printf("TID1 = %d\n",tid1);
	}
	
    }
}
}

    


	bitonicMerge(lo, cnt, dir);


  }

}


/** function recBitonicSort() 
    first produces a bitonic sequence by recursively sorting 
    its two halves in opposite sorting orders, and then
    calls bitonicMerge to make them in the same order 
 **/
void ser_recBitonicSort(int lo, int cnt, int dir) {
int k;

  if (cnt>1) {
      k=cnt/2;
//if(k>(N/4))
//printf("Thread id = %d\n", k);
      ser_recBitonicSort(lo, k, ASCENDING);
      
      ser_recBitonicSort(lo+k, k, DESCENDING);
           
   
      bitonicMerge(lo, cnt, dir);

  }

}






/** function sort() 
   Caller of recBitonicSort for sorting the entire array of length N 
   in ASCENDING order
 **/
void sort() {
  recBitonicSort(0, N, ASCENDING);
}



/*
  imperative version of bitonic sort
*/
void impBitonicSort() {

  int i,j,k,ij;
#pragma omp parallel num_threads(NT) shared(a,chunk)  private(k,j,i,ij)
{
  for (k=2; k<=N;k=2*k) {
    for (j=k>>1; j>0; j=j>>1) {

      #pragma omp for schedule(dynamic,chunk) 
      for (i=0; i<N; i++) {
	 ij=i^j;
	if ((ij)>i) {
	  if ((i&k)==0 && a[i] > a[ij]) 
	      exchange(i,ij);
	  if ((i&k)!=0 && a[i] < a[ij])
	      exchange(i,ij);
	}
      }
    }
  }
}

}

