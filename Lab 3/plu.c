#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

#define N 8
double A[N][N + 1];
pthread_barrier_t barrier;

int print_matrix()
{
    int i, j;
    printf("------------------------------------\n");
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N + 1; j++)
            printf("%6.2f  ", A[i][j]);
        printf("\n");
    }
}

// LU decomposition function
int *lu()
{
    int i, j, k, m, itemp;
    double max;
    double temp;
    double si;

    // LU decomposition loop
    for (k = 0; k < n; k++)
    {
        max = 0;
        printf("partial pivoting by row %d\n", k);
        for (i = k; i < n; i++)
        {
            if (max < fabs(A[i][k]))
            { // partial pivoting
                max = fabs(A[i][k]);
                j = i;
            }
        }
        if (max == 0)
        {
            printf("zero pivot: singular A matrix\n");
            exit(1);
        }

        // swap P[k] and P[j];
        itemp = P[k];
        P[k] = P[j];
        P[j] = itemp;

        // swap row A[k] and row A[j]
        printf("swap row %d and row %d of A\n", k, j);
        for (m = 0; m < n; m++)
        {
            temp = A[k][m];
            A[k][m] = A[j][m];
            A[j][m] = temp;
        }
        print('A', A);
        getchar();

        //swap L[k][0,k-2] and L[j][0,k-2]
        for (m = 0; m < k - 2; m++)
        {
            temp = L[k][m];
            L[k][m] = L[j][m];
            L[j][m] = temp;
        }

        // compute L U entries
        U[k][k] = A[k][k];
        for (i = k + 1; i < n; i++)
        {
            L[i][k] = A[i][k] / U[k][k];
            U[k][i] = A[k][i];
        }

        // row reductions on A
        printf("row reducations of A by row %d\n", k);
        for (i = k + 1; i < n; i++)
        {
            for (m = k + 1; m < n; m++)
            {
                A[i][m] -= L[i][k] * U[k][m];
            }
        }
        print('A', A);
        print('L', L);
        print('U', U);
        printP();
        getchar();
    }
}

int main(int argc, char *argv[])
{
    int i, j;
    double sum;

    pthread_t threads[N];

    printf("main: initialize matrix A[N][N+1] as [A|B]\n");
    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++)
            A[i][j] = 1.0;
    for (i = 0; i < N; i++)
        A[i][N - i - 1] = 1.0 * N;
    for (i = 0; i < N; i++)
    {
        A[i][N] = (N * (N + 1)) / 2 + (N - i) * (N - 1);
    }
    print_matrix(); // show initial matrix [A|B]

    pthread_barrier_init(&barrier, NULL, N);

    printf("main: create N=%d working threads\n", N);
    for (i = 0; i < N; i++)
    {
        pthread_create(&threads[i], NULL, lu, (void *)i);
    }
    printf("main: wait for all %d working threads to join\n", N);
    for (i = 0; i < N; i++)
    {
        pthread_join(threads[i], NULL);
        printf("Thread %d joined \n", i);
    }
}