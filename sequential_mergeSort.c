#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void merge(int arr[], int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    int* L = (int*)malloc(n1 * sizeof(int));
    int* R = (int*)malloc(n2 * sizeof(int));

    for(int i = 0; i < n1; i++) L[i] = arr[left + i];
    for(int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;
    while(i < n1 && j < n2){
        if(L[i] <= R[j]) arr[k++] = L[i++];
        else arr[k++] = R[j++];
    }
    while(i < n1) arr[k++] = L[i++];
    while(j < n2) arr[k++] = R[j++];

    free(L);
    free(R);
}

void mergeSort(int arr[], int left, int right){
    if(left < right){
        int mid = left + (right - left)/2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid+1, right);
        merge(arr, left, mid, right);
    }
}

int isSorted(int arr[], int n){
    for(int i = 1; i < n; i++)
        if(arr[i-1] > arr[i]) return 0;
    return 1;
}

int main() {
    int n = 1000000; 
    int* arr = (int*)malloc(n * sizeof(int));

    srand(time(NULL));
    for(int i = 0; i < n; i++) arr[i] = rand();

    clock_t start = clock();
    mergeSort(arr, 0, n-1);
    clock_t end = clock();

    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Sequential Merge Sort Time: %f seconds\n", time_taken);

    if(isSorted(arr, n))
        printf("Array is sorted correctly!\n");
    else
        printf("Sorting failed!\n");

    free(arr);
    return 0;
}