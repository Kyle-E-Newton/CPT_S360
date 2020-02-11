/** C4.3.c: matrix sum by threads with mutex lock **/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define  N   128
int A[N][N];

long int total = 0;           // global total
pthread_mutex_t *m;           // mutex pointer

void *func(void *arg)         // working thread function
{
   int i, row, sum = 0;
   pthread_t tid = pthread_self(); // get thread ID number
   row = (int)arg;                 // get row number from arg
   printf("Thread %d [%lu] computes sum of row %d\n", row, tid, row);
   for (i=0; i<N; i++)       // compute partial sum of A[row]in 
       sum += A[row][i];
   printf("Thread %d [%lu] update total with %d\n", row, tid, sum);
   pthread_mutex_lock(m);
     total += sum;           // update global total inside a CR
   pthread_mutex_unlock(m);  
   printf("total = %ld\n", total);
}

int main (int argc, char *argv[])
{
   pthread_t thread[N];
   int i, j, r;
   void *status;

   struct timeval t1, t2;
   gettimeofday(&t1, NULL);

   printf("Main: initialize A matrix\n");
   for (i=0; i<N; i++){
     for (j=0; j<N; j++){
         A[i][j] = i*N + j + 1;
         printf("%4d ", A[i][j]);
     }
     printf("\n");
   }
   // create a mutex m
   m = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
   pthread_mutex_init(m, NULL); // initialize mutex m 
   printf("Main: create %d threads\n", N);
   for(i=0; i<N; i++) {
      pthread_create(&thread[i], NULL, func, (void *)i); 
   }
   printf("Main: try to join with threads\n");
   for(i=0; i<N; i++) {
      pthread_join(thread[i], &status);
      printf("Main: joined with %d [%lu]: status=%d\n", 
              i, thread[i], (int)status);
   }
   printf("Main: tatal = %ld\n", total);
   pthread_mutex_destroy(m); // destroy mutex m

   gettimeofday(&t2, NULL);
   printf("t2: sec=%ld usec=%ld\n", t2.tv_sec, t2.tv_usec);
   printf("t1: sec=%ld usec=%ld\n", t1.tv_sec, t1.tv_usec);
   printf("usec used = %ld\n", t2.tv_usec - t1.tv_usec);

   pthread_exit(NULL);
}

