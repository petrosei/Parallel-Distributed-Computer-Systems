/*

 Parallel implementation of bitonix sort using pThreads.

 Author: Evangelakos Petros 

 Based on:

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
#include <pthread.h>

struct timeval startwtime, endwtime;
double seq_time;


int N;          // data array size
int NT;		// number of Threads
int *a;         // data array to be sorted
int count = 1;
pthread_mutex_t mutexsum;

const int ASCENDING  = 1;
const int DESCENDING = 0;


void init(void);
void print(void);
void sort(void);
void test(void);
void exchange(int i, int j);
void compare(int i, int j, int dir);
void bitonicMerge(int lo, int cnt, int dir);
void recBitonicSort(int lo, int cnt, int dir);
void PrecBitonicSort(int lo, int cnt, int dir);
void impBitonicSort(void);



/* 
Imperative Implementation of bitonic sort using pthrteads
	

*/
struct thread_data{
   int  thread_id;
   int  shared_k;
   int  shared_j;
};



/* 
Recursive Implementation of bitonic sort using pthrteads
        

*/
struct thread_data_rec{
   int  thread_id;
   int  thread_lo;
   int  thread_cnt;
   int  thread_dir;
};

//struct thread_data_rec data_rec;

void *Prec(void *threadarg)
 {
    int tid,lo,cnt,dir;
    struct thread_data_rec *my_data;
    my_data = (struct thread_data *) threadarg;
    tid = my_data->thread_id;
    lo = my_data->thread_lo;
    cnt = my_data->thread_cnt;
    dir = my_data->thread_dir;
   //printf("Limits of thread %d, N = %d\n",tid,N);
   // if(tid%2==0)
	PrecBitonicSort(lo, cnt, ASCENDING);
    //else
    //    PrecBitonicSort(lo, cnt, DESCENDING); 
    
    
    
   pthread_exit(NULL);

 }







/** the main program **/ 
int main(int argc, char **argv) {

  if (argc != 3) {
    printf("Usage: %s q\n  where n=2^q is problem size (power of two)\n", 
	   argv[0]);
    exit(1);
  }

  N = 1<<atoi(argv[1]);
  NT = atoi(argv[2]);
  a = (int *) malloc(N * sizeof(int));
  

  init();
  gettimeofday (&startwtime, NULL);
  sort();
  gettimeofday (&endwtime, NULL);

  seq_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6 + endwtime.tv_sec - startwtime.tv_sec);

  printf("Recursive wall clock time = %f\n", seq_time);

  test();



    /* Last thing that main() should do */
    pthread_exit(NULL);


  // print();
}

/** -------------- SUB-PROCEDURES  ----------------- **/ 

/** procedure test() : verify sort results **/
void test() {
  int pass = 1;
  int count = 0;
  int pos;
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
    for (i=lo; i<lo+k; i++)
      compare(i, i+k, dir);
    bitonicMerge(lo, k, dir);
    bitonicMerge(lo+k, k, dir);
  }
}







/** function recBitonicSort() 
    first produces a bitonic sequence by recursively sorting 
    its two halves in opposite sorting orders, and then
    calls bitonicMerge to make them in the same order 
 **/
void recBitonicSort(int lo, int cnt, int dir) {
  if (cnt>1) {
    int k=cnt/2;
    recBitonicSort(lo, k, ASCENDING);
    recBitonicSort(lo+k, k, DESCENDING);
    bitonicMerge(lo, cnt, dir);
  }
}


void PrecBitonicSort(int lo, int cnt, int dir) {
  if (cnt>1) {
    int k=cnt/2;
  if(count+1<=NT){
    pthread_mutex_lock (&mutexsum);
    count++;
    pthread_mutex_unlock (&mutexsum);
    int rc,t;
    void *status;
    struct thread_data_rec data_rec; 
    pthread_t threads[1];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    
    data_rec.thread_lo = lo;
    data_rec.thread_cnt = k;
    data_rec.thread_dir = dir;
    data_rec.thread_id = 0;

       rc = pthread_create(&threads[0], &attr, Prec, (void *)&data_rec);
       if (rc){
          printf("ERROR; return code from pthread_create() is %d\n", rc);
          exit(-1);
       }

	
      PrecBitonicSort(lo+k, k, DESCENDING);      

       rc = pthread_join(threads[0], &status);
       if (rc) {
          printf("ERROR; return code from pthread_join() is %d\n", rc);
          exit(-1);
          }
       pthread_attr_destroy(&attr);
   }
   else{
    
    recBitonicSort(lo, k, ASCENDING);
    recBitonicSort(lo+k, k, DESCENDING);
   }
    bitonicMerge(lo, cnt, dir);
  }
}



/** function sort() 
   Caller of recBitonicSort for sorting the entire array of length N 
   in ASCENDING order
 **/
void sort() {
  PrecBitonicSort(0, N, ASCENDING);
}


