/***********Quicksort Sequential***********/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define N 10        

int a[N] = {5,1,6,4,7,2,9,8,0,3};  // unsorted data

void swap(int *a, int *b) {
    int t = *a;
    *a = *b;
    *b = t;
}

int partition(int arr[], int low, int high) {
    int pivot = arr[high];
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
        if(arr[j] < pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}

void Qs(int arr[], int low, int high) {
    if(low < high) {
        int pi = partition(arr, low, high);

        Qs(arr, low, pi - 1);
        Qs(arr, pi + 1, high);
    }
}

int main() {
    struct timeval t1, t2;
    gettimeofday(&t1, NULL);

    Qs(a, 0, N - 1);

    gettimeofday(&t2, NULL);

    printf("t2: sec=%ld usec=%ld\n", t2.tv_sec, t2.tv_usec);
    printf("t1: sec=%ld usec=%ld\n", t1.tv_sec, t1.tv_usec);
    printf("usec used = %ld\n", t2.tv_usec - t1.tv_usec);
}