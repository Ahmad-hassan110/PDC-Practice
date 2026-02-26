#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void merge(int arr[], int left, int mid, int right){
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

    free(L); free(R);
}

void mergeSort(int arr[], int left, int right){
    if(left < right){
        int mid = left + (right - left)/2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid+1, right);
        merge(arr, left, mid, right);
    }
}

void mergeArrays(int *arr, int *temp, int n, int chunk_size, int size){
    int i, j, k;
    for(i = 0; i < size; i++){
        int left = i * chunk_size;
        int right = left + chunk_size - 1;
        if(right >= n) right = n - 1;
        merge(arr, left, (left+right)/2, right);
    }

    int *res = (int*)malloc(n * sizeof(int));
    int idx[size];
    for(i = 0; i < size; i++) idx[i] = i*chunk_size;

    for(k = 0; k < n; k++){
        int min_val = __INT_MAX__;
        int min_idx = -1;
        for(j = 0; j < size; j++){
            if(idx[j] < (j+1)*chunk_size && idx[j] < n && arr[idx[j]] < min_val){
                min_val = arr[idx[j]];
                min_idx = j;
            }
        }
        res[k] = min_val;
        idx[min_idx]++;
    }

    for(i = 0; i < n; i++) arr[i] = res[i];
    free(res);
}

int isSorted(int arr[], int n){
    for(int i = 1; i < n; i++)
        if(arr[i-1] > arr[i]) return 0;
    return 1;
}

/*int main(int argc, char *argv[]){
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n = 1000000; 
    int *arr = NULL;
    int chunk_size = n / size;
    int *sub_arr = (int*)malloc(chunk_size * sizeof(int));

    if(rank == 0){
        arr = (int*)malloc(n * sizeof(int));
        srand(42);
        for(int i = 0; i < n; i++) arr[i] = rand();
    }

    MPI_Scatter(arr, chunk_size, MPI_INT, sub_arr, chunk_size, MPI_INT, 0, MPI_COMM_WORLD);

    double start = MPI_Wtime();
    mergeSort(sub_arr, 0, chunk_size-1);
    double end_local = MPI_Wtime();

    MPI_Gather(sub_arr, chunk_size, MPI_INT, arr, chunk_size, MPI_INT, 0, MPI_COMM_WORLD);

    if(rank == 0){
        double end = MPI_Wtime();
        mergeArrays(arr, NULL, n, chunk_size, size);

        printf("MPI Merge Sort with %d processes completed in %f seconds\n", size, end - start);
        if(isSorted(arr, n))
            printf("Array is sorted correctly!\n");
        else
            printf("Sorting failed!\n");
        free(arr);
    }

    free(sub_arr);
    MPI_Finalize();
    return 0;
}*/

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    printf("Hello from rank %d out of %d on %s\n", rank, size, processor_name);

    MPI_Finalize();
    return 0;
}