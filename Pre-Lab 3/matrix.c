/***********Matrix Sum Sequential***********/
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define N 128
int A[N][N];

int main()
{
    struct timeval t1, t2;
    gettimeofday(&t1, NULL);

    int i, j, r;

    printf("Main: initialize A matrix\n");
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            A[i][j] = i * N + j + 1;
            printf("%4d ", A[i][j]);
        }
        printf("\n");
    }

    int sum = computeSum(A, N);
    printf("%d \n", sum);

    gettimeofday(&t2, NULL);
    printf("t2: sec=%ld usec=%ld\n", t2.tv_sec, t2.tv_usec);
    printf("t1: sec=%ld usec=%ld\n", t1.tv_sec, t1.tv_usec);
    printf("usec used = %ld\n", t2.tv_usec - t1.tv_usec);
}

int computeSum(int arr[N][N], int size) {
    int sum = 0;
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++) {
            sum = sum + arr[i][j];
        }
    }
    return sum;
}